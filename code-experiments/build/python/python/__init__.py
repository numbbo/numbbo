"""Experimentation module of the COCO - COmparing Continuous Optimizers -
framework.

The module provides benchmark test beds in the `Suite` class
and output data facilities in the `Observer` class.

See the documentation of the `Suite` class:

>>> import cocoex as ex
>>> help(ex.Suite)  # doctest: +ELLIPSIS
Help on class Suite...
>>> print(ex.known_suite_names)  # doctest: +ELLIPSIS
[...

A more complete example use case can be found in the `example_experiment.py`
file.
"""
from __future__ import absolute_import, division, print_function, unicode_literals
from . import solvers, utilities
try:
    from . import _interface
    from ._interface import Suite as _Suite, Observer as _Observer
    from ._interface import known_suite_names, log_level
except Exception as _e:
    from . import interface as _interface
    # print("numbbo/code-experiments/build/python/python/__init__.py: could not import '_interface', trying 'interface'", _e)
    from .interface import Suite as _Suite, Observer as _Observer
    from .interface import known_suite_names, log_level
del absolute_import, division, print_function, unicode_literals

# from .utilities import about_equal
# from .exceptions import NoSuchProblemException, InvalidProblemException

__all__ = ['Observer', 'Suite', 'known_suite_names', 'default_observers']

def default_observers(update=None):
    """return a map from suite names to default observer names.

    This function can also be used to update this map using
    a `dict` or a `list` of key-value pairs.
    """
    # this is a function only to make the doc available and
    # because @property doesn't work on module level
    _default_observers.update(update or {})
    return _default_observers
_default_observers = {
    'bbob': 'bbob',
    'bbob-biobj': 'bbob-biobj',
    'bbob-biobj-ext': 'bbob-biobj',
    'bbob-constrained': 'bbob',
    'bbob-largescale': 'bbob',
    'bbob-mixint': 'bbob',
    'bbob-biobj-mixint': 'bbob-biobj',
    'toy-socket': 'bbob',
    'toy-socket-biobj': 'bbob-biobj',
    }

class Suite(_Suite):
    """Suite of benchmark problems.

    Input arguments to `Suite` are `name: str`, `instance: str`, `options: str`,
    and passed to the respective C code (see `coco.h`).

    >>> import cocoex as ex
    >>> suite = ex.Suite("bbob", "", "")
    >>> f = suite.next_problem()
    >>> assert f.number_of_objectives == 1
    >>> assert f.evaluations == 0
    >>> print("f([1,2]) = %.11f" % f([1,2]))
    f([1,2]) = 90.00369408000
    >>> assert f.evaluations == 1

    Sweeping through all problems is as simple as:

    >>> import cocoex as ex
    >>> suite = ex.Suite("bbob-biobj", "", "")
    >>> observer = ex.Observer("bbob-biobj", "result_folder:doctest")
    >>> for fun in suite:
    ...     if fun.index == 0:
    ...         print("Number of objectives %d, %d, %d" %
    ...                 (fun.number_of_objectives,
    ...                  suite.number_of_objectives[0],
    ...                  suite.number_of_objectives[-1]))
    ...     fun.observe_with(observer)
    ...     assert fun.evaluations == 0
    ...     assert fun.number_of_objectives == suite.number_of_objectives[0]
    ...     # run run run using fun  # doctest: +ELLIPSIS
    Number of objectives 2, 2, 2...

    In the example, an observer was added to produce output data for the
    COCO post-processing.

    The following example runs the entire bbob2009 benchmark suite
    on random search:

    >>> import numpy as np
    >>> from cocoex import Suite, Observer
    ...
    >>> MAX_FE = 22  # max f-evaluations
    >>> def random_search(f, lb, ub, m):  # don't use m >> 1e5 with this implementation
    ...     candidates = lb + (ub - lb) * np.random.rand(m, len(lb))
    ...     return candidates[np.argmin([f(x) for x in candidates])]
    ...
    >>> solver = random_search
    >>> suite = Suite("bbob", "year:2009", "")
    >>> observer = Observer("bbob",
    ...              "result_folder: %s_on_%s" % (solver.__name__, "bbob2009"))
    >>> for fun in suite:
    ...     assert fun.evaluations == 0
    ...     if fun.dimension >= 10:
    ...         break
    ...     print('Current problem index = %d' % fun.index)
    ...     fun.observe_with(observer)
    ...     assert fun.evaluations == 0
    ...     solver(fun, fun.lower_bounds, fun.upper_bounds, MAX_FE)
    ...     # data should be now in the "exdata/random_search_on_bbob2009" folder
    ...     assert fun.evaluations == MAX_FE  # depends on the solver
    ...     # doctest: +ELLIPSIS
    Current problem index = 0...
    >>> #
    >>> # Exactly the same using another looping technique:
    >>> for id in suite.ids():
    ...     fun = suite.get_problem(id, observer)
    ...     _ = solver(fun, fun.lower_bounds, fun.upper_bounds, MAX_FE)
    ...     print("Evaluations on %s: %d" % (fun.name, fun.evaluations))
    ...     fun.free()  # this is absolutely necessary here
    ...     # doctest: +ELLIPSIS
    Evaluations on ...

    We can select a single function, say BBOB f9 in 20D, of a given suite like:

    >>> import cocoex as ex
    >>> suite = ex.Suite("bbob", "", "dimensions:20 instance_indices:1")
    >>> len(suite)
    24
    >>> f9 = suite.get_problem(8)
    >>> x = f9.initial_solution  # a copy of a feasible point
    >>> all(x == 0)
    True

    See module attribute `cocoex.known_suite_names` for known suite names:

    >>> import cocoex as ex
    >>> for suite_name in ex.known_suite_names:
    ...     suite = ex.Suite(suite_name, "", "")
    ...     print(suite.dimensions)
    ...     for f in suite:
    ...         assert f.dimension in suite.dimensions
    ...         assert f.evaluations == 0
    ...         # doctest: +ELLIPSIS
    [2, 3, 5, 10, 20, 40]...

    See file `example_experiment.py` for a full example use case.

    Details: depending on the benchmark suite and observer, only one problem can
    be open at a time. Using `get_problem` without `free` or mixing the use of
    `next_problem` and `get_problem` may not be possible. For example, in this
    case the "bbob" observer is known to lead to a crash of the Python
    interpreter.

    See also `Observer` and `example_experiment.py`.
    """
    def __init__(self, suite_name, suite_instance, suite_options):
        """``suite_instance`` and ``suite_options`` can be empty strings."""
        # this __init__ defines the arguments for _Suite.__cinit__,
        # which is called implicitly. Calling the super class init fails in Python 3.
        # super(Suite, self).__init__(suite_name, suite_instance, suite_options)
        # _Suite.__cinit__(self, suite_name, suite_instance, suite_options)
    def reset(self):
        """reset to original state, affecting `next_problem()`,
        `current_problem`, `current_index`"""
        super(Suite, self).reset()
    def next_problem(self, observer=None):
        """return the "next" problem in this `Suite`.

        return the first problem on the first call or after
        `reset` ().

        `next_problem` serves to sweep through the `Suite` smoothly.
        """
        return super(Suite, self).next_problem(observer)
    def get_problem(self, id, observer=None):
        """return a `Problem` instance, by default unobserved, using ``id: str``
        or index (where ``id: int``) to identify the desired problem.

        All values between zero and `len(self) - 1` are valid index values:

        >>> import cocoex as ex
        >>> suite = ex.Suite("bbob-biobj", "", "")
        >>> for index in range(len(suite)):
        ...     problem = suite.get_problem(index)
        ...     # work work work using problem
        ...     problem.free()

        A shortcut for `suite.get_problem(index)` is `suite[index]`, they are
        synonym.

        Details:
            - Here an `index` takes values between 0 and `len(self) - 1` and can in
              principle be different from the problem index in the benchmark suite.
    
            - This call does not affect the state of the `current_problem` and
              `current_index` attributes.
    
            - For some suites and/or observers, the `free()` method of the problem
              must be called before the next call of `get_problem`. Otherwise Python
              might just silently die, which is e.g. a known issue of the "bbob"
              observer.

        See also `ids`.
        """
        return super(Suite, self).get_problem(id, observer)

    def get_problem_by_function_dimension_instance(self, function, dimension, instance, observer=None):
        """return a `Problem` instance, by default unobserved, using function,
        dimension and instance to identify the desired problem.

        If a suite contains multiple problems with the same function, dimension
        and instance, the first corresponding problem is returned.

        >>> import cocoex as ex
        >>> suite = ex.Suite("bbob-biobj", "", "")
        >>> problem = suite.get_problem_by_function_dimension_instance(1, 2, 3)
        >>> # work work work using problem
        >>> problem.free()

        Details:
            - Function, dimension and instance are integer values from 1 on.
    
            - This call does not affect the state of the `current_problem` and
              `current_index` attributes.
    
            - For some suites and/or observers, the `free()` method of the problem
              must be called before the next call of
              `get_problem_by_function_dimension_instance`. Otherwise Python might
              just silently die, which is e.g. a known issue of the "bbob" observer.
        """
        return super(Suite, self).get_problem_by_function_dimension_instance(
                function, dimension, instance, observer)

    def __getitem__(self, key):
        """`self[i]` is a synonym for `self.get_problem(i)`, see `get_problem`
        """
        return self.get_problem(key)

    def free(self):
        """free underlying C structures"""
        super(Suite, self).free()

    def find_problem_ids(self, *args, **kwargs):
        """has been renamed to `ids`"""
        raise NotImplementedError(
            "`find_problem_ids()` has been renamed to `ids()`")

    def ids(self, *id_snippets, **kwargs):  # get_problem=False, verbose=False):
        """`ids(*id_snippets, get_problem=False, verbose=False)`
        return all problem IDs that contain all of the `id_snippets`.

        An ID can be used for indexing, that is, when calling the method
        `get_problem(id)`.

        If `get_problem is True`, the problem for the first matching ID is
        returned.

        >>> import cocoex as ex
        >>> s = ex.Suite("bbob", "", "")
        >>> s.ids("f001", "d10", "i01")
        ['bbob_f001_i01_d10']

        We can sweep through all instances of the ellipsoidal function f10
        in 20-D of the BBOB suite like this:

        >>> import cocoex as ex
        >>> suite = ex.Suite("bbob", "", "")
        >>> ids = suite.ids("f010", "d20")
        >>> used_indices = []
        >>> for p in suite:
        ...     if p.id in ids:
        ...         # work work work with problem `p`
        ...         used_indices.append(p.index)
        >>> print(used_indices)
        [1575, 1576, 1577, 1578, 1579, 1580, 1581, 1582, 1583, 1584, 1585, 1586, 1587, 1588, 1589]

        A desired problem can also be filtered out during creation:

        >>> import cocoex as ex
        >>> f9 = ex.Suite("bbob", "",
        ...               "function_indices:9 dimensions:20 instance_indices:1-5")[0]
        >>> print(f9.id)
        bbob_f009_i01_d20

        """
        return super(Suite, self).ids(*id_snippets, **kwargs)

    @property
    def current_problem(self):
        """current "open/active" problem to be benchmarked"""
        return super(Suite, self).current_problem
    @property
    def current_index(self):
        """index in the enumerator of all problems in this suite.

        Details: To get the index in the underlying C implementation, which
        usually matches `current_index` one-to-one, use:

        >>> import cocoex as ex
        >>> suite = ex.Suite("bbob", "", "")
        >>> suite.current_index is None
        True
        >>> suite.next_problem().id[-17:].lower()
        'bbob_f001_i01_d02'
        >>> suite.current_index, suite.indices[suite.current_index]
        (0, 0)

        """
        return super(Suite, self).current_index
    @property
    def problem_names(self):
        """list of problem names in this `Suite`, see also `ids`"""
        return super(Suite, self).problem_names
    @property
    def dimensions(self):
        """list of problem dimensions occuring at least once in this `Suite`"""
        return super(Suite, self).dimensions
    @property
    def number_of_objectives(self):
        """list of number of objectives occuring in this `Suite`"""
        return super(Suite, self).number_of_objectives
    @property
    def indices(self):
        """list of all problem indices, deprecated.
        
        These values are (only) used to call the underlying C structures.
        Indices used in the Python interface run between 0 and `len(self)`.
        """
        return super(Suite, self).indices
    @property
    def name(self):
        """name of this suite as used to instantiate the suite via `Suite(name, ...)`"""
        return super(Suite, self).name
    @property
    def instance(self):
        """instance of this suite as used to instantiate the suite via
        `Suite(name, instance, ...)`"""
        return super(Suite, self).instance
    @property
    def options(self):
        """options for this suite as used to instantiate the suite via
        `Suite(name, instance, options)`"""
        return super(Suite, self).options
    @property
    def info(self):
        return str(self)

class Observer(_Observer):
    """Observer which can be "attached to" one or several problems, however not
    necessarily at the same time.

    The typical observer records data to be used in the COCO post-processing
    module `cocopp` afterwards.

    >>> import cocoex as ex
    >>> suite = ex.Suite("bbob", "", "")
    >>> assert len(suite) == 2160
    >>> f = suite.get_problem(33)
    >>> assert f.id.endswith('f003_i04_d02')
    >>> observer = ex.Observer("bbob",
    ...                        "result_folder: doctest")
    >>> f.observe_with(observer)  # the same as observer.observe(f)  # doctest: +ELLIPSIS
    <cocoex...
    >>> # work work work with observed f
    >>> f.free()

    Details
    -------

        - ``f.free()`` in the above example must be called before to observe
          another problem with the "bbob" observer. Otherwise the Python
          interpreter will crash due to an error raised from the C code.
    
        - Due to technical sublties between Python/Cython/C, the pointer to the
          underlying C observer is passed by global assignment with
          `_update_current_observer_global()`

    """

    def __init__(self, name, options):
        """``options`` can be a string or a `dict`"""
        # this __init__ defines the arguments for _Observer.__cinit__,
        # which is called implicitly
        # super(Observer, self).__init__(name, options)  # fails (only) in Python 3

    def observe(self, problem):
        """`observe(problem)` let `self` observe the `problem: Problem` by
        calling `problem.observe_with(self)`.
        """
        problem.observe_with(self)
        return self

    @property
    def name(self):
        """name of the observer as used with `Observer(name, ...)` to instantiate
        `self` before
        """
        return super(Observer, self).name
    @property
    def options(self):
        return super(Observer, self).options
    @property
    def state(self):
        return super(Observer, self).state
    @property
    def result_folder(self):
        """name of the output folder.

        This name may not be the same as input option `result_folder`.
        """
        return super(Observer, self).result_folder

# this definition is copy-edited from interface, solely to pass docstrings to pydoctor
class Problem(_interface.Problem):
    """`Problem` instances are usually generated using class `Suite`.
    
    The main feature of a problem instance is that it is callable, returning the
    objective function value when called with a candidate solution as input.
    
    It provides other useful properties and methods like `dimension`,
    `number_of_constraints`, `observe_with`, `initial_solution_proposal`...

    """
    def __init__(self):
        super(Problem, self).__init__()
    def constraint(self, x):
        """return constraint values for `x`. 

        By convention, constraints with values <= 0 are satisfied.
        """
        return super(Problem, self).constraint(x)

    def logger_biobj_feed_solution(self, evaluation, y):
        """Feed the given solution to logger_biobj in order to reconstruct its
        output.

        Return 1 if the given solution updated the archive and 0 otherwise.

        Used by preprocessing when updating the .info, .dat and .tdat files
        with new indicator reference values.
        """
        return super(Problem, self).logger_biobj_feed_solution(evaluation, y)


    def add_observer(self, observer):
        """`add_observer(self, observer: Observer)`, see `observe_with`.
        """
        return self.observe_with(observer)

    def observe_with(self, observer):
        """``observe_with(self, observer: Observer)`` attaches an `Observer`
        instance to this problem.

        Attaching an observer can be considered as wrapping the observer
        around the problem. For the observer to be finalized, the problem
        must be free'd (implictly or explicitly).

        Return the observed problem `self`.

        Details: `observer` can be `None`, in which case nothing is done.

        See also: class `Observer`
        """
        return super(Problem, self).observe_with(observer)

    def _f0(self, x):
        """"inofficial" interface to `self` with target f-value of zero. """
        return self(x) - self.final_target_fvalue1

    def initial_solution_proposal(self, restart_number=None):
        """return feasible initial solution proposals.

        For unconstrained problems, the proposal is different for each
        consecutive call without argument and for each `restart_number`
        and may be different under repeated calls with the same
        `restart_number`. ``self.initial_solution_proposal(0)`` is the
        same as ``self.initial_solution``.

        Conceptual example::

            # given: a suite instance, a budget, and fmin
            for problem in suite:
                # restart until budget is (over-)exhausted
                while problem.evaluations < budget and not problem.final_target_hit:
                    fmin(problem, problem.initial_solution_proposal())

        Details: by default, the first proposal is the domain middle or
        the (only) known feasible solution.
        Subsequent proposals are coordinate-wise sampled as the sum
        of two iid random variates uniformly distributed within the
        domain boundaries. On the ``'bbob'`` suite their density is
        0.2 * (x / 5 + 1) for x in [-5, 0] and
        0.2 * (1 - x / 5) for x in [0, 5] and zero otherwise.

        """
        return super(Problem, self).initial_solution_proposal(restart_number)
    @property
    def initial_solution(self):
        """return feasible initial solution"""
        return super(Problem, self).initial_solution()
    @property
    def observers(self):
        """list of observers wrapped around this problem"""
        return super(Problem, self).list_of_observers
    @property
    def is_observed(self):
        """problem ``p`` is observed ``p.is_observed`` times.

        See also: the list of observers in property `observers`.
        """
        return super(Problem, self).is_observed

    @property
    def number_of_variables(self):  # this is cython syntax, not known in Python
        # this is a class definition which is instantiated automatically!?
        """Number of variables this problem instance expects as input."""
        return super(Problem, self).number_of_variables
    @property
    def dimension(self):
        """alias for `number_of_variables` of the input space"""
        return self.number_of_variables
    @property
    def number_of_objectives(self):
        "number of objectives, if equal to 1, call returns a scalar"
        return super(Problem, self).number_of_objectives
    @property
    def number_of_constraints(self):
        "number of constraints"
        return super(Problem, self).number_of_constraints
    @property
    def lower_bounds(self):
        """depending on the test bed, these are not necessarily strict bounds
        """
        return super(Problem, self).lower_bounds
    @property
    def upper_bounds(self):
        """depending on the test bed, these are not necessarily strict bounds
        """
        return super(Problem, self).upper_bounds
    @property
    def evaluations(self):
        """number of times this `Problem` instance was evaluated"""
        return super(Problem, self).evaluations()
    @property
    def final_target_hit(self):
        """return 1 if the final target is known and has been hit, 0 otherwise
        """
        return super(Problem, self).final_target_hit(self.problem)
    @property
    def final_target_fvalue1(self):
        return super(Problem, self).final_target_fvalue1(self.problem)
    @property
    def best_observed_fvalue1(self):
        return super(Problem, self).best_observed_fvalue1()

    def free(self, force=False):
        """Free the given test problem.

        Not strictly necessary (unless, possibly, for the observer). `free`
        ensures that all files associated with the problem are closed as
        soon as possible and any memory is freed. After free()ing the
        problem, all other operations are invalid and will raise an
        exception.
        """
        super(Problem, self).free(force)

    @property
    def id(self):
        "ID as string without spaces or weird characters"
        return super(Problem, self).id

    @property
    def id_function(self):
        "function number inferred from `id`"
        return super(Problem, self).id_function

    @property
    def id_instance(self):
        "instance number inferred from `id`"
        return super(Problem, self).id_instance

    @property
    def name(self):
        """human readible short description with spaces"""
        return super(Problem, self).name

    @property
    def index(self):
        """problem index in the benchmark `Suite` of origin"""
        return super(Problem, self).index

    @property
    def suite(self):
        """benchmark suite this problem is from"""
        return super(Problem, self).suite

    @property
    def info(self):
        """human readible info, alias for ``str(self)``.

        The format of this info string is not guarantied and may change
        in future.

        See also: ``repr(self)``
        """
        return str(self)

def log_level(level=None):
    """``log_level(level=None)`` return current log level and
    set new log level if ``level is not None and level``.

    :param level: must be 'error' or 'warning' or 'info' or 'debug', listed
        with increasing verbosity, or '' which doesn't change anything.

    """
    return _interface.log_level(level)

