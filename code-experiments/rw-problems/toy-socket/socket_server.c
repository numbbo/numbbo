/**
 * The socket server in C.
 *
 * Uses the toy_evaluator to evaluate problems from the toy-socket suite. Change code below to
 * connect it to other evaluators (for other suites).
 */

/* The winsock2.h header *needs* to be included before windows.h! */
#if 0 & ( defined(_WIN32) || defined(_WIN64) || defined(__MINGW64__) || defined(__CYGWIN__))
#include <winsock2.h>
#if _MSC_VER
#pragma comment(lib, "ws2_32.lib") /* Winsock library */
#endif
#define WINSOCK 1
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define WINSOCK 0
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT 7251
#define MESSAGE_SIZE 8000   /* Should be large enough to contain a number of x-values */
#define RESPONSE_SIZE 256   /* Should be large enough to contain a couple of objective values */
#define SUITE_NAME_SIZE 64  /* Should be large enough to contain the name of a suite */
#define PRECISION_Y 16      /* Precision used to write objective values */
#define LOG_MESSAGES 1      /* Set to 1 (0) to (not) print the messages */

#include "toy_evaluator.c"  /* Use the toy_evaluator for evaluation */

/**
 * Parses the message and calls an evaluator to compute the evaluation. Then constructs a response.
 * Returns the response.
 */
char *evaluate_message(char *message) {

  char suite_name[SUITE_NAME_SIZE];
  char *response = "";
  size_t number_of_objectives, i;
  size_t function, instance, dimension;
  double *x, *y;
  int read_count;
  int char_count, offset = 0;

  /* Parse the message
   *
   * char_count is used to count how many characters are read and offset moves the pointer
   * along the message accordingly
   */
  if ((read_count = sscanf(message, "n %s o %lu f %lu i %lu d %lu x%*c%n",
      suite_name, &number_of_objectives, &function, &instance, &dimension, &char_count)) != 5) {
    fprintf(stderr, "evaluate_message(): Failed to read beginning of the message %s", message);
    fprintf(stderr, "(read %d instead of %d items)", read_count, 5);
    exit(EXIT_FAILURE);
  }
  x = malloc(dimension * sizeof(double));
  offset = char_count;
  for (i = 0; i < dimension; i++) {
    if (sscanf(message + offset, "%lf%*c%n", &x[i], &char_count) != 1) {
      fprintf(stderr, "evaluate_message(): Failed to read message %s", message);
      exit(EXIT_FAILURE);
    }
    offset += char_count;
  }

  /* Evaluate x and save the result to y
   *
   * Add here additional evaluators
   */
  y = malloc(number_of_objectives * sizeof(double));
  if ((strcmp(suite_name, "toy-socket") == 0) || (strcmp(suite_name, "toy-socket-biobj") == 0)) {
    /* Use the toy_evaluator to compute the value(s) of y from x */
    evaluate(suite_name, number_of_objectives, function, instance, dimension, x, y);
  } else {
    fprintf(stderr, "evaluate_message(): Suite %s not supported", suite_name);
    exit(EXIT_FAILURE);
  }
  free(x);

  /* Construct the response */
  response = (char *) malloc(RESPONSE_SIZE);
  if (number_of_objectives == 1) {
    sprintf(response, "%.*e", PRECISION_Y, y[0]);
  } else if (number_of_objectives == 2) {
    sprintf(response, "%.*e %.*e", PRECISION_Y, y[0], PRECISION_Y, y[1]);
  } else {
    fprintf(stderr, "evaluate_message(): %lu objectives not supported (yet)", number_of_objectives);
  }
  free(y);

  return response;
}

/**
 * Starts the server.
 *
 * Should be working for different platforms.
 */
void socket_server_start(void) {

  struct sockaddr_in address;
  int address_size = sizeof(address);
  int yes = 1;
  char message[MESSAGE_SIZE];
  char *response;

#if WINSOCK
  WSADATA wsa;
  SOCKET sock;
  int message_len;

  /* Initialize Winsock */
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    fprintf(stderr, "socket_server_start(): Winsock initialization failed: %d", WSAGetLastError());
  }

  /* Create a socket file descriptor */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    fprintf(stderr, "socket_server_start(): Could not create socket: %d", WSAGetLastError());
  }

  /* Forcefully attach socket to the port */
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
    perror("socket_server_start(): Socket could not be attached to the port");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; /* "any address" in IPV4 */
  address.sin_port = htons(PORT);

  /* Bind */
  if (bind(sock, (struct sockaddr*) &address, sizeof(address)) < 0) {
    perror("socket_server_start(): Bind failed");
    exit(EXIT_FAILURE);
  }

  /* Listen */
  if (listen(sock, 3) < 0) {
    perror("socket_server_start(): Listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Server started, listening on port %d\n", PORT);

  while (1) {
    /* Accept an incoming connection */
    if ((new_sock = accept(sock, (struct sockaddr*) &address, (socklen_t*) &address_size)) < 0) {
      perror("socket_server_start(): Accept failed");
      exit(EXIT_FAILURE);
    }

    /* Read the message */
    if ((message_len = recv(new_sock, message, MESSAGE_SIZE, 0)) == SOCKET_ERROR) {
      perror("socket_server_start(): Receive failed");
      exit(EXIT_FAILURE);
    }
#if LOG_MESSAGES == 1
    printf("Received message: %s (length %ld)\n", message, message_len);
#endif

    /* Parse the message and evaluate its contents using an evaluator */
    response = evaluate_message(message);

    /* Send the response */
    send(new_sock, response, strlen(response), 0);
#if LOG_MESSAGES == 1
    printf("Sent response %s (length %ld)\n", response, strlen(response));
#endif
    free(response);
    closesocket(new_sock);
  }
#else
  int sock, new_sock;
  long message_len;

  /* Create a socket file descriptor */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket_server_start(): Socket creation error");
    exit(EXIT_FAILURE);
  }

  /* Forcefully attach socket to the port */
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) {
    perror("socket_server_start(): Socket could not be attached to the port");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; /* "any address" in IPV4 */
  address.sin_port = htons(PORT);

  /* Bind */
  if (bind(sock, (struct sockaddr*) &address, sizeof(address)) < 0) {
    perror("socket_server_start(): Bind failed");
    exit(EXIT_FAILURE);
  }

  /* Listen */
  if (listen(sock, 3) < 0) {
    perror("socket_server_start(): Listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Server started, listening on port %d\n", PORT);

  while (1) {
    /* Accept an incoming connection */
    if ((new_sock = accept(sock, (struct sockaddr*) &address, (socklen_t*) &address_size)) < 0) {
      perror("socket_server_start(): Accept failed");
      exit(EXIT_FAILURE);
    }

    /* Read the message */
    if ((message_len = read(new_sock, message, MESSAGE_SIZE)) < 0) {
      perror("socket_server_start(): Receive failed");
      exit(EXIT_FAILURE);
    }
#if LOG_MESSAGES == 1
    printf("Received message: %s (length %ld)\n", message, message_len);
#endif

    /* Parse the message and evaluate its contents using an evaluator */
    response = evaluate_message(message);

    /* Send the response */
    send(new_sock, response, strlen(response), 0);
#if LOG_MESSAGES == 1
    printf("Sent response %s (length %ld)\n", response, strlen(response));
#endif
    free(response);
  }
#endif
}

int main(void)
{
  socket_server_start();
  return 0;
}
