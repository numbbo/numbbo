#include <stdio.h>
#include <string.h>
#include "coco_platform.h"
#include "coco_string.c"

#define RESPONSE_SIZE 256 /* Should be large enough to contain a couple of objective values */

/**
 * @brief Data type needed for socket communication (used by the suites that need it).
 */
typedef struct {
  unsigned short port;  /**< @brief The port for communication with the external evaluator. */
  char *host_name;      /**< @brief The host name for communication with the external evaluator. */
  int precision_x;      /**< @brief Precision used to write the x-values to the external evaluator. */
} socket_communication_data_t;

/**
 * @brief Frees the memory of a socket_communication_data_t object.
 */
static void socket_communication_data_free(void *stuff) {

  socket_communication_data_t *data;

  assert(stuff != NULL);
  data = (socket_communication_data_t *) stuff;
  if (data->host_name != NULL) {
    coco_free_memory(data->host_name);
  }
}

static socket_communication_data_t *socket_communication_data_initialize(const char *suite_options) {

  socket_communication_data_t *data;
  data = (socket_communication_data_t *) coco_allocate_memory(sizeof(*data));

  data->host_name = coco_strdup("127.0.0.1");
  if (coco_options_read_string(suite_options, "host_name", data->host_name) == 0) {
    strcpy(data->host_name, "127.0.0.1");
  }

  data->port = 7251;
  if (coco_options_read(suite_options, "port", "%hu", &(data->port)) == 0) {
  }

  data->precision_x = 8;
  if (coco_options_read_int(suite_options, "precision_x", &(data->precision_x)) != 0) {
    if ((data->precision_x < 1) || (data->precision_x > 32)) {
      data->precision_x = 8;
      coco_warning("socket_communication_data_initialize(): Adjusted precision_x value to %d",
          data->precision_x);
    }
  }
  return data;
}

/**
 * Prepares and returns the message for the evaluator. The message has the following format:
 * n <n> o <o> f <f> i <i> d <d> x <x1> <x2> ... <xd>
 * Where
 * <n> is the suite name (for example, "toy-socket")
 * <o> is the number of objectives
 * <f> is the function number
 * <i> is the instance number
 * <d> is the problem dimension
 * <xk> is the k-th value of x (there should be exactly d x values)
 */
static char *socket_communication_get_message(const char *suite_name,
                                              const size_t number_of_objectives,
                                              const size_t function,
                                              const size_t instance,
                                              const size_t dimension,
                                              const double *x,
                                              const int precision_x) {
  char *message, *tmp_string;
  size_t i;

  message = coco_strdupf("n %s o %lu f %lu i %lu d %lu x",
      suite_name, number_of_objectives, function, instance, dimension);
  for (i = 0; i < dimension; i++) {
    tmp_string = message;
    message = coco_strdupf("%s %.*e", message, precision_x, x[i]);
    coco_free_memory(tmp_string);
  }
  return message;
}

/**
 * Reads the evaluator response and saves it into y. The response should have the following format:
 * <y1> ... <ym>
 * Where
 * <yk> is the value of the k-th objective
 */
static void socket_communication_save_response(const char *response,
                                               const long response_size,
                                               const size_t expected_number_of_objectives,
                                               double *y) {

  if (response_size < 1) {
    coco_error("socket_communication_save_response(): Incorrect response %s (size %d)",
        response, response_size);
  }

  if (expected_number_of_objectives == 1) {
    if (sscanf(response, "%lf", &y[0]) != 1) {
      coco_error("socket_communication_save_response(): Could not read 1 objective value");
    }
  }
  else if (expected_number_of_objectives == 2) {
    if (sscanf(response, "%lf %lf", &y[0], &y[1]) != 2) {
      coco_error("socket_communication_save_response(): Could not read 2 objective values");
    }
  }
  else {
    coco_error("socket_communication_save_response(): %lu objective not supported (yet)",
        expected_number_of_objectives);
  }

}

/**
 * Sends the message to the external evaluator through sockets. The external evaluator must be running
 * a server using the same port.
 *
 * Should be working for different platforms.
 */
static void socket_communication_evaluate(const char* host_name, const unsigned short port,
    const char *message, const size_t expected_number_of_objectives, double *y) {

  char response[RESPONSE_SIZE];

#if WINSOCK
  WSADATA wsa;
  SOCKET sock;
  SOCKADDR_IN serv_addr;
  int response_len;

  /* Initialize Winsock */
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    coco_error("socket_communication_evaluate(): Winsock initialization failed: %d", WSAGetLastError());
  }

  /* Create a socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    coco_error("socket_communication_evaluate(): Could not create socket: %d", WSAGetLastError());
  }

  serv_addr.sin_addr.s_addr = inet_addr(host_name);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  /* Connect to the evaluator */
  if (connect(sock, (SOCKADDR *) &serv_addr, sizeof(serv_addr)) < 0) {
    coco_error("socket_communication_evaluate(): Connection failed\nIs the server running?");
  }

  /* Send message */
  if (send(sock, message, (int)strlen(message) + 1, 0) < 0) {
    coco_error("socket_communication_evaluate(): Send failed: %d", WSAGetLastError());
  }
  coco_debug("Sent message: %s", message);

  /* Receive the response */
  if ((response_len = recv(sock, response, RESPONSE_SIZE, 0)) == SOCKET_ERROR) {
    coco_error("socket_communication_evaluate(): Receive failed: %d", WSAGetLastError());
  }
  coco_debug("Received response: %s (length %d)", response, response_len);

  socket_communication_save_response(response, response_len, expected_number_of_objectives, y);

  closesocket(sock);
  WSACleanup();
#else
  int sock;
  struct sockaddr_in serv_addr;
  long response_len;

  /* Create a socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    coco_error("socket_communication_evaluate(): Socket creation error");
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  /* Convert IPv4 and IPv6 addresses from text to binary form */
  if (inet_pton(AF_INET, host_name, &serv_addr.sin_addr) <= 0) {
    coco_error("socket_communication_evaluate(): Invalid address / Address not supported");
  }

  /* Connect to the evaluator */
  if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
    coco_error("socket_communication_evaluate(): Connection failed\nIs the server running?");
  }

  /* Send message */
  if (send(sock, message, strlen(message) + 1, 0) < 0) {
    coco_error("socket_communication_evaluate(): Send failed");
  }
  coco_debug("Sent message: %s", message);

  /* Receive the response */
  response_len = read(sock, response, RESPONSE_SIZE);
  coco_debug("Received response: %s (length %ld)", response, response_len);

  socket_communication_save_response(response, response_len, expected_number_of_objectives, y);
  close(sock);
#endif
}

/**
 * @brief Calls the external evaluator to evaluate x.
 */
static void socket_evaluate(coco_problem_t *problem, const double *x, double *y) {

  char *message;
  socket_communication_data_t *data = (socket_communication_data_t *) problem->suite->data;

  message = socket_communication_get_message(
      problem->suite->suite_name,
      problem->number_of_objectives,
      problem->suite_dep_function,
      problem->suite_dep_instance,
      problem->number_of_variables,
      x,
      data->precision_x
  );
  socket_communication_evaluate(
      data->host_name,
      data->port,
      message,
      problem->number_of_objectives,
      y
  );
  coco_free_memory(message);
}

