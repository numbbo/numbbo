# Using socket communication for external evaluation 

Socket communication between COCO and an external evaluator can be used to ease inclusion 
of new suites of problems into COCO, for example, those that implement real-world problems. 
Socket communication is demonstrated on the example of two test suites, `toy-socket` and 
`toy-socket-biobj` – the first contains single-objective and the second bi-objective optimization problems. 

An external evaluator is basically a server that needs to run and listen for messages
from the client (COCO). At each solution evaluation, COCO sends a message with information
on the problem and the solution to the external evaluator. The external evaluator then 
computes and returns the objective values to COCO as a response message. 

Two external evaluators are available - one in Python and the other in C (see the files
in this folder). Both are implemented using two files - one takes care of the socket 
communication (files `socket_server.py` and `socket_server.c` for Python and C, respectively) 
and the other of the actual evaluation of solutions (files `toy_socket_evaluator.py` and 
`toy_socket_evaluator.c` in the `toy-socket` folder for Python and C, respectively). 

It should be rather easy to add aditional (real-world) evaluators to the socket servers, 
look for text starting with `ADD HERE` in the files `socket_server.py` and `socket_server.c`. 

## Running experiments

### Running a prepared experiment on the `toy-socket` suite

**NOTE: Not yet well tested on different platforms**

By calling

````
python do.py test-socket-python
````

form the root directory of the repository, the Python socket server will be started in a
separate console window and the Python example experiment will be run on the `toy-socket` suite. 

### Starting the external evaluator server 

The external evaluator should be run in a different console window than the example experiment.

The external evaluator in C can be compiled and run through `do.py` (at the root directory
of the repository) using:

````
python do.py run-socket-c
````


The external evaluator in Python can be run through `do.py` (at the root directory
of the repository) using:

````
python do.py run-socket-python
````

To view output from the servers, they need to be run directly using `./socket_server` or
`python socket_server.py` from this folder.

### Running the example experiments

**IMPORTANT: The servers need to be up and running before COCO experiments are started!**

After the external evaluator server has started, the example experiments in the available
languages can be invoked as usual using `do.py`. Note that the `bbob` suite should be 
changed to `toy-socket` in the example experiments!
