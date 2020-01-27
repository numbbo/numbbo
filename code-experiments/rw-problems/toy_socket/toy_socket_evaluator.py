"""
An evaluator for problems from the toy-socket suite to demonstrate external evaluation in Python
"""


def evaluate_toy_socket(suite_name, num_objectives, func, instance, x):
    if suite_name == 'toy-socket' and num_objectives == 1:
        y = instance * 100.0
        if func == 1:
            # Function 1 is the sum of all values
            for xi in x:
                y += xi
            return [y]
        elif func == 2:
            # Function 2 is the sum of squares of all values
            for xi in x:
                y += xi * xi
            return [y]
        else:
            raise('Suite {} has no function {}'.format(suite_name, func))
    elif suite_name == 'toy-socket-biobj' and num_objectives == 2:
        y1 = y2 = instance * 100.0
        if func == 1:
            # Objective 1 is the sum of all values
            for xi in x:
                y1 += xi
            # Objective 2 is the sum of squares of all values
            for xi in x:
                y2 += xi * xi
            return [y1, y2]
        else:
            raise ValueError('Suite {} has no function {}'.format(suite_name, func))
    else:
        raise ValueError('Suite {} cannot have {} objectives'.format(suite_name, num_objectives))

