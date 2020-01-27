"""
The socket server in Python.

Uses the toy_socket_evaluator to evaluate problems from the toy-socket suite. Change code below to
connect it to other evaluators (for other suites) -- see occurrences of 'ADD HERE'.
"""
import sys
import socket
from toy_socket.toy_socket_evaluator import evaluate_toy_socket
# ADD HERE imports from other evaluators, for example
# from my_suite.my_evaluator import evaluate_my_suite

HOST = ''            # Symbolic name, meaning all available interfaces
PORT = 7251          # Arbitrary non-privileged port
MESSAGE_SIZE = 8000  # Should be large enough to contain a number of x-values
PRECISION_Y = 16     # Precision used to write objective values


def evaluate_message(message):
    """Parses the message and calls an evaluator to compute the evaluation. Then constructs a
    response. Returns the response."""
    try:
        # Make sure to remove and null endings
        message = message.split('\x00', 1)[0]
        # Parse the message
        msg = message.split(' ')
        suite_name = msg[msg.index('n') + 1]
        func = int(msg[msg.index('f') + 1])
        dimension = int(msg[msg.index('d') + 1])
        instance = int(msg[msg.index('i') + 1])
        num_objectives = int(msg[msg.index('o') + 1])
        x = [float(m) for m in msg[msg.index('x') + 1:]]
        if len(x) != dimension:
            raise ValueError('Number of x values {} does not match dimension {}'.format(len(x),
                                                                                        dimension))

        # Find the right evaluator
        if 'toy-socket' in suite_name:
            evaluate = evaluate_toy_socket
        # ADD HERE the function for another evaluator, for example
        # elif 'my-suite' in suite_name:
        #     evaluate = evaluate_my_suite
        else:
            raise ValueError('Suite {} not supported'.format(suite_name))
        # Evaluate x and save the result to y
        y = evaluate(suite_name, num_objectives, func, instance, x)
        # Construct the response
        response = ''
        for yi in y:
            response += '{:.{p}e} '.format(yi, p=PRECISION_Y)
        return str.encode(response)
    except Exception as e:
        print('Error within message evaluation: {}'.format(e))
        raise e


def socket_server_start(silent=False):
    s = None
    try:
        # Create socket
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Bind socket to local host and port
        try:
            s.bind((HOST, PORT))
        except socket.error as e:
            print('Bind failed: {}'.format(e))
            raise e

        # Start listening on socket
        s.listen(1)
        print('Server ready, listening on port {}'.format(PORT))

        # Talk with the client
        while True:
            try:
                # Wait to accept a connection - blocking call
                conn, addr = s.accept()
            except socket.error as e:
                print('Accept failed: {}'.format(e))
                raise e
            except KeyboardInterrupt or SystemExit:
                print('Server terminated')
                return 0
            with conn:
                # Read the message
                message = conn.recv(MESSAGE_SIZE).decode("utf-8")
                if not silent:
                    print('Received message: {}'.format(message))
                # Parse the message and evaluate its contents using an evaluator
                response = evaluate_message(message)
                # Send the response
                conn.sendall(response)
                if not silent:
                    print('Sent response: {}'.format(response.decode("utf-8")))
    except Exception as e:
        print('Error: {}'.format(e))
        raise e
    finally:
        if s is not None:
            print('Closing socket')
            s.close()


if __name__ == '__main__':
    silent = False
    if len(sys.argv) == 2:
        if sys.argv[1] == 'silent':
            silent = True
        else:
            print('Ignoring input option {}'.format(sys.argv[1]))
    elif len(sys.argv) > 2:
        print('Too many options (at most one supporte), ignoring all')
    socket_server_start(silent=silent)
