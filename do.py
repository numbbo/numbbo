#!/usr/bin/env python
## -*- mode: python -*-
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import sys
import os
from os.path import join
import shutil
import tempfile
import subprocess
from subprocess import STDOUT
import platform
import time
import glob


## Change to the root directory of repository and add our tools/
## subdirectory to system wide search path for modules.
os.chdir(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.abspath(join('code-experiments', 'tools')))

from amalgamate import amalgamate
from cocoutils import make, run, python, check_output
from cocoutils import copy_file, expand_file, write_file
from cocoutils import git_version, git_revision

CORE_FILES = ['code-experiments/src/coco_random.c',
              'code-experiments/src/coco_suite.c',
              'code-experiments/src/coco_observer.c',
              'code-experiments/src/coco_archive.c'
             ]

MATLAB_FILES = ['cocoCall.m', 'cocoEvaluateFunction.m', 'cocoObserver.m',
                'cocoObserverFree.m', 'cocoProblemAddObserver.m',
                'cocoProblemFinalTargetHit.m', 'cocoProblemFree.m',
                'cocoProblemGetDimension.m', 'cocoProblemGetEvaluations.m',
                'cocoProblemGetId.m', 'cocoProblemGetInitialSolution.m',
                'cocoProblemGetLargestValuesOfInterest.m',
                'cocoProblemGetName.m', 'cocoProblemGetNumberOfObjectives.m',
                'cocoProblemGetSmallestValuesOfInterest.m',
                'cocoProblemIsValid.m', 'cocoProblemRemoveObserver.m',
                'cocoSetLogLevel.m', 'cocoSuite.m', 'cocoSuiteFree.m',
                'cocoSuiteGetNextProblem.m', 'cocoSuiteGetProblem.m']

_verbosity = False
# Do not suppress build messages unless specifically requested
_build_verbosity = True

################################################################################
## C
def build_c():
    """ Builds the C source code """
    global RELEASE
    amalgamate(CORE_FILES + ['code-experiments/src/coco_runtime_c.c'],
               'code-experiments/build/c/coco.c', RELEASE,
               {"COCO_VERSION": git_version(pep440=True)})
    expand_file('code-experiments/src/coco.h',
                'code-experiments/build/c/coco.h',
                {"COCO_VERSION": git_version(pep440=True)})
    copy_file('code-experiments/build/c/coco.c',
              'code-experiments/examples/bbob2009-c-cmaes/coco.c')
    expand_file('code-experiments/build/c/coco.h',
                'code-experiments/examples/bbob2009-c-cmaes/coco.h',
                {'COCO_VERSION': git_version(pep440=True)})
    write_file(git_revision(), "code-experiments/build/c/REVISION")
    write_file(git_version(), "code-experiments/build/c/VERSION")
    if 11 < 3:
        python('code-experiments/build/c', ['make.py', 'clean'], verbose=_build_verbosity)
        python('code-experiments/build/c', ['make.py', 'all'], verbose=_build_verbosity)
    else:
        make("code-experiments/build/c", "clean", verbose=_build_verbosity)
        make("code-experiments/build/c", "all", verbose=_build_verbosity)


def run_c():
    """ Builds and runs the example experiment in C """
    build_c()
    try:
        run('code-experiments/build/c', ['./example_experiment'], verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)


def test_c():
    """ Builds and runs unit tests, integration tests and an example experiment test in C """
    build_c()
    # Perform unit tests
    build_c_unit_tests()
    run_c_unit_tests()
    # Perform integration tests
    build_c_integration_tests()
    run_c_integration_tests()
    # Perform example experiment tests
    build_c_example_tests()
    run_c_example_tests()


def test_c_unit():
    """ Builds and runs unit tests in C """
    build_c()
    # Perform unit tests
    build_c_unit_tests()
    run_c_unit_tests()


def test_c_integration():
    """ Builds and runs integration tests in C """
    build_c()
    # Perform integration tests
    build_c_integration_tests()
    run_c_integration_tests()


def test_c_example():
    """ Builds and runs an example experiment test in C """
    build_c()
    # Perform example tests
    build_c_example_tests()
    run_c_example_tests()


def build_c_unit_tests():
    """ Builds unit tests in C """
    library_path = 'code-experiments/test/unit-test/lib'
    library_dir = ''
    file_name = ''
    if 'win32' in sys.platform:
        file_name = 'cmocka.dll'
        if '64' in platform.machine():
            library_dir = 'win64'
        elif ('32' in platform.machine()) or ('x86' in platform.machine()):
            if 'cygwin' in os.environ['PATH']:
                library_dir = 'win32_cygwin'
            else:
                library_dir = 'win32_mingw'
    elif 'linux' in sys.platform:
        file_name = 'libcmocka.so'
        if 'Ubuntu' in platform.linux_distribution():
            library_dir = 'linux_ubuntu'
        elif 'Fedora' in platform.linux_distribution():
            library_dir = 'linux_fedora'
    elif 'darwin' in sys.platform:  # Mac
        library_dir = 'macosx'
        file_name = 'libcmocka.dylib'

    if len(library_dir) > 0:
        copy_file(os.path.join(library_path, library_dir, file_name),
                  os.path.join('code-experiments/test/unit-test', file_name))
    copy_file('code-experiments/build/c/coco.c', 'code-experiments/test/unit-test/coco.c')
    expand_file('code-experiments/src/coco.h', 'code-experiments/test/unit-test/coco.h',
                {'COCO_VERSION': git_version(pep440=True)})
    make("code-experiments/test/unit-test", "clean", verbose=_build_verbosity)
    make("code-experiments/test/unit-test", "all", verbose=_build_verbosity)


def run_c_unit_tests():
    """ Runs unit tests in C """
    try:
        run('code-experiments/test/unit-test', ['./unit_test'], verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)


def build_c_integration_tests():
    """ Builds integration tests in C """
    copy_file('code-experiments/build/c/coco.c',
              'code-experiments/test/integration-test/coco.c')
    expand_file('code-experiments/src/coco.h',
                'code-experiments/test/integration-test/coco.h',
                {'COCO_VERSION': git_version(pep440=True)})
    copy_file('code-experiments/src/bbob2009_testcases.txt', 'code-experiments/test/integration-test/bbob2009_testcases.txt')
    copy_file('code-experiments/src/bbob2009_testcases2.txt', 'code-experiments/test/integration-test/bbob2009_testcases2.txt')
    make("code-experiments/test/integration-test", "clean", verbose=_build_verbosity)
    make("code-experiments/test/integration-test", "all", verbose=_build_verbosity)


def run_c_integration_tests():
    """ Runs integration tests in C """
    try:
        run('code-experiments/test/integration-test',
            ['./test_coco', 'bbob2009_testcases.txt'], verbose=_verbosity)
        run('code-experiments/test/integration-test',
            ['./test_coco', 'bbob2009_testcases2.txt'], verbose=_verbosity)
        run('code-experiments/test/integration-test',
            ['./test_instance_extraction'], verbose=_verbosity)
        run('code-experiments/test/integration-test',
            ['./test_biobj'], verbose=_verbosity)
        run('code-experiments/test/integration-test',
            ['./test_bbob-constrained'], verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)


def build_c_example_tests():
    """ Builds an example experiment test in C """
    if os.path.exists('code-experiments/test/example-test'):
        shutil.rmtree('code-experiments/test/example-test')
        time.sleep(1)  # Needed to avoid permission errors for os.makedirs
    os.makedirs('code-experiments/test/example-test')
    copy_file('code-experiments/build/c/coco.c', 'code-experiments/test/example-test/coco.c')
    expand_file('code-experiments/src/coco.h', 'code-experiments/test/example-test/coco.h',
                {'COCO_VERSION': git_version(pep440=True)})
    copy_file('code-experiments/build/c/example_experiment.c',
              'code-experiments/test/example-test/example_experiment.c')
    copy_file('code-experiments/build/c/Makefile.in',
              'code-experiments/test/example-test/Makefile.in')
    copy_file('code-experiments/build/c/Makefile_win_gcc.in',
              'code-experiments/test/example-test/Makefile_win_gcc.in')
    make("code-experiments/test/example-test", "clean", verbose=_build_verbosity)
    make("code-experiments/test/example-test", "all", verbose=_build_verbosity)


def run_c_example_tests():
    """ Runs an example experiment test in C """
    try:
        run('code-experiments/test/example-test', ['./example_experiment'],
            verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)


def leak_check():
    """ Performs a leak check in C """
    build_c()
    build_c_integration_tests()
    os.environ['CFLAGS'] = '-g -Os'
    valgrind_cmd = ['valgrind', '--error-exitcode=1', '--track-origins=yes',
                    '--leak-check=full', '--show-reachable=yes',
                    './test_coco', 'bbob2009_testcases.txt']
    run('code-experiments/test/integration-test', valgrind_cmd, verbose=_verbosity)
    valgrind_cmd = ['valgrind', '--error-exitcode=1', '--track-origins=yes',
                    '--leak-check=full', '--show-reachable=yes',
                    './test_biobj', 'leak_check']
    run('code-experiments/test/integration-test', valgrind_cmd, verbose=_verbosity)
    valgrind_cmd = ['valgrind', '--error-exitcode=1', '--track-origins=yes',
                    '--leak-check=full', '--show-reachable=yes',
                    './test_bbob-constrained', 'leak_check']
    run('code-experiments/test/integration-test', valgrind_cmd, verbose=_verbosity)


################################################################################
## Python 2
def install_error(e):
    exception_message = e.output.splitlines()
    formatted_message = ["|" + " " * 77 + "|"]
    for line in exception_message:
        while len(line) > 75:
            formatted_message.append("| " + line[:75] + " |")
            line = line[75:]
        formatted_message.append("| " + line.ljust(75) + " |")
    print("""
An exception occurred while trying to install packages.

A common reason for this error is insufficient access rights
to the installation directory. The original exception message
is as follows:

/----------------------------< EXCEPTION MESSAGE >----------------------------\\
{0}
\\-----------------------------------------------------------------------------/

To fix an access rights issue, you may try the following:

- Run the same command with "install-user" as additional argument.
  To get further help run do.py without a specific command.

- On *nix systems or MacOS, run the same command with a preceded "sudo ".

- Gain write access to the installation directory by changing
  access permissions or gaining administrative access.

""".format("\n".join(formatted_message)))
    return True

def install_postprocessing(package_install_option = []):
    ''' Installs the COCO postprocessing as python module. '''
    global RELEASE
    expand_file(join('code-postprocessing', 'setup.py.in'),
                join('code-postprocessing', 'setup.py'),
                {'COCO_VERSION': git_version(pep440=True)})
    # copy_tree('code-postprocessing/latex-templates', 'code-postprocessing/cocopp/latex-templates')
    python('code-postprocessing', ['setup.py', 'install']
           + package_install_option, verbose=_verbosity,
           custom_exception_handler=install_error)

def uninstall_postprocessing():
    run('.', ['pip', 'uninstall', 'cocopp', '-y'], verbose=_verbosity)

def test_suites(args):
    """regression test on suites via Python"""
    if not args:
        args = ['2']
    test_python(  # this is a list of [folder, args] pairs
        [['code-experiments/test/regression-test',
          ['test_suites.py', arg]] for arg in args
        ] + [
            ['code-experiments/build/python',
             ['coco_test.py', 'bbob2009_testcases.txt', 'bbob2009_testcases2.txt']
            ]
        ])

def _prep_python():
    global RELEASE
    amalgamate(CORE_FILES + ['code-experiments/src/coco_runtime_c.c'],
               'code-experiments/build/python/cython/coco.c',
               RELEASE, {"COCO_VERSION": git_version(pep440=True)})
    expand_file('code-experiments/src/coco.h',
                'code-experiments/build/python/cython/coco.h',
                {'COCO_VERSION': git_version(pep440=True)})
    copy_file('code-experiments/src/bbob2009_testcases.txt',
              'code-experiments/build/python/bbob2009_testcases.txt')
    copy_file('code-experiments/src/bbob2009_testcases2.txt',
              'code-experiments/build/python/bbob2009_testcases2.txt')
    copy_file('code-experiments/build/python/README.md',
              'code-experiments/build/python/README.txt')
    expand_file('code-experiments/build/python/setup.py.in',
                'code-experiments/build/python/setup.py',
                {'COCO_VERSION': git_version(pep440=True)})  # hg_version()})
    # if 'darwin' in sys.platform:  # a hack to force cythoning
    #     run('code-experiments/build/python/cython', ['cython', 'interface.pyx'])


def build_python(package_install_option=[]):
    _prep_python()
    ## Force distutils to use Cython
    # os.environ['USE_CYTHON'] = 'true'
    # python('code-experiments/build/python', ['setup.py', 'sdist'])
    # python(join('code-experiments', 'build', 'python'),
    #        ['setup.py', 'install', '--user'])
    python(join('code-experiments', 'build', 'python'), ['setup.py', 'install']
           + package_install_option, custom_exception_handler=install_error)
    # os.environ.pop('USE_CYTHON')


def run_python(test=False, package_install_option = []):
    """ Builds and installs the Python module `cocoex` and runs the
    `example_experiment.py` as a simple test case. If `test` is True,
    it runs, in addition, the tests in `coco_test.py`."""
    build_python(package_install_option=package_install_option)
    try:
        if test:
            python(os.path.join('code-experiments', 'build', 'python'), ['coco_test.py'])
        python(os.path.join('code-experiments', 'build', 'python'),
               ['example_experiment.py', 'bbob'])
    except subprocess.CalledProcessError:
        sys.exit(-1)


def run_sandbox_python(directory, script_filename=
                       os.path.join('code-experiments', 'build', 'python',
                                    'example_experiment.py')):
    """run a python script after building and installing `cocoex` in a new
    environment."""
    _prep_python()
    python('code-experiments/build/python',
           ['setup.py', 'check', '--metadata', '--strict'], verbose=_verbosity)
    ## Now install into a temporary location, run test and cleanup
    python_temp_home = tempfile.mkdtemp(prefix="coco")
    python_temp_lib = os.path.join(python_temp_home, "lib", "python")
    try:
        ## We setup a custom "homedir" here into which we install our
        ## coco extension and then use that temporary installation for
        ## the tests. Otherwise we would run the risk of contaminating
        ## the Python installation of the build/test machine.
        os.makedirs(python_temp_lib)
        os.environ['PYTHONPATH'] = python_temp_lib
        os.environ['USE_CYTHON'] = 'true'
        python('code-experiments/build/python',
               ['setup.py', 'install', '--home', python_temp_home],
               verbose=_verbosity, custom_exception_handler=install_error)
        python(directory, [script_filename])
        os.environ.pop('USE_CYTHON')
        os.environ.pop('PYTHONPATH')
    except subprocess.CalledProcessError:
        sys.exit(-1)
    finally:
        shutil.rmtree(python_temp_home)


def test_python(args=(['code-experiments/build/python', ['coco_test.py', 'None']],)):
    _prep_python()
    python('code-experiments/build/python',
           ['setup.py', 'check', '--metadata', '--strict'],
           verbose=_verbosity)
    ## Now install into a temporary location, run test and cleanup
    python_temp_home = tempfile.mkdtemp(prefix="coco")
    python_temp_lib = os.path.join(python_temp_home, "lib", "python")
    try:
        ## We setup a custom "homedir" here into which we install our
        ## coco extension and then use that temporary installation for
        ## the tests. Otherwise we would run the risk of contaminating
        ## the Python installation of the build/test machine.
        os.makedirs(python_temp_lib)
        os.environ['PYTHONPATH'] = python_temp_lib
        os.environ['USE_CYTHON'] = 'true'
        python('code-experiments/build/python',
               ['setup.py', 'install', '--home', python_temp_home],
               verbose=_verbosity, custom_exception_handler=install_error)
        for folder, more_args in args:
            python(folder, more_args, verbose=_verbosity)
        # python('code-experiments/build/python',
        #        ['coco_test.py', 'bbob2009_testcases.txt'], verbose=_verbosity)
        # python('code-experiments/build/python',
        #        ['coco_test.py', 'bbob2009_testcases2.txt'], verbose=_verbosity)
        os.environ.pop('USE_CYTHON')
        os.environ.pop('PYTHONPATH')
    except subprocess.CalledProcessError:
        sys.exit(-1)
    finally:
        shutil.rmtree(python_temp_home)


################################################################################
## Matlab
def build_matlab():
    """Builds MATLAB example in build/matlab/ but not the one in examples/."""

    global RELEASE
    amalgamate(CORE_FILES + ['code-experiments/src/coco_runtime_matlab.c'],
               'code-experiments/build/matlab/coco.c',
               RELEASE, {"COCO_VERSION": git_version(pep440=True)})
    expand_file('code-experiments/src/coco.h',
                'code-experiments/build/matlab/coco.h',
                {'COCO_VERSION': git_version(pep440=True)})
    write_file(git_revision(), "code-experiments/build/matlab/REVISION")
    write_file(git_version(), "code-experiments/build/matlab/VERSION")
    run('code-experiments/build/matlab',
        ['matlab', '-nodisplay', '-nosplash', '-r', 'setup, exit'],
        verbose=_verbosity)


def run_matlab():
    """ Builds and runs the example experiment in build/matlab/ in MATLAB """
    print('CLEAN\tmex files from code-experiments/build/matlab/')
    # remove the mex files for a clean compilation first
    for filename in glob.glob('code-experiments/build/matlab/*.mex*'):
        os.remove(filename)
    # amalgamate, copy, and build
    build_matlab()
    wait_for_compilation_to_finish('./code-experiments/build/matlab/cocoCall')
    # run after compilation finished
    run('code-experiments/build/matlab',
        ['matlab', '-nodisplay', '-nosplash', '-r', 'exampleexperiment, exit'],
        verbose=_verbosity)


def is_compiled(filenameprefix):
    """Returns true iff a file 'filenameprefix.mex*' exists."""

    # get all files with the given prefix
    files = glob.glob(filenameprefix + '.*')
    # return true iff one of the files contains 'mex'
    ret = False
    for f in files:
        if '.mex' in f:
            ret = True
    return ret


def wait_for_compilation_to_finish(filenameprefix):
    """Waits until filenameprefix.c is compiled into a mex file.

    Needed because under Windows, a MATLAB call is typically non-blocking
    and thus, the experiments would be started before the compilation is over.
    """

    print('Wait for compilation to finish', end='')
    while not is_compiled(filenameprefix):
        time.sleep(2)
        print('.', end='')
    print(' ')


def build_matlab_sms():
    """Builds the SMS-EMOA in MATLAB """
    global RELEASE
    destination_folder = 'code-experiments/examples/bbob-biobj-matlab-smsemoa'
    # amalgamate and copy files
    amalgamate(CORE_FILES + ['code-experiments/src/coco_runtime_matlab.c'],
               join(destination_folder, 'coco.c'), RELEASE,
               {"COCO_VERSION": git_version(pep440=True)})
    expand_file('code-experiments/src/coco.h', join(destination_folder, 'coco.h'),
                {'COCO_VERSION': git_version(pep440=True)})
    for f in MATLAB_FILES:
        copy_file(join('code-experiments/build/matlab/', f), join(destination_folder, f))
    write_file(git_revision(), join(destination_folder, "REVISION"))
    write_file(git_version(), join(destination_folder, "VERSION"))
    copy_file('code-experiments/build/matlab/cocoCall.c', join(destination_folder, 'cocoCall.c'))
    # compile
    run(destination_folder, ['matlab', '-nodisplay', '-nosplash', '-r', 'setup, exit'])


def run_matlab_sms():
    """ Builds and runs the SMS-EMOA in MATLAB """
    print('CLEAN\tmex files from code-experiments/examples/bbob-biobj-matlab-smsemoa/')
    # remove the mex files for a clean compilation first
    for filename in glob.glob('code-experiments/examples/bbob-biobj-matlab-smsemoa/*.mex*'):
        os.remove(filename)
    # amalgamate, copy, and build
    build_matlab_sms()
    wait_for_compilation_to_finish('./code-experiments/examples/bbob-biobj-matlab-smsemoa/paretofront')
    # run after compilation finished
    run('code-experiments/examples/bbob-biobj-matlab-smsemoa',
        ['matlab', '-nodisplay', '-nosplash', '-r', 'run_smsemoa_on_bbob_biobj, exit'],
        verbose=_verbosity)


################################################################################
## Octave
def build_octave():
    """Builds example in build/matlab/ with GNU Octave."""

    global RELEASE
    amalgamate(CORE_FILES + ['code-experiments/src/coco_runtime_c.c'],
               'code-experiments/build/matlab/coco.c', RELEASE,
               {"COCO_VERSION": git_version(pep440=True)})
    expand_file('code-experiments/src/coco.h', 'code-experiments/build/matlab/coco.h',
                {'COCO_VERSION': git_version(pep440=True)})
    write_file(git_revision(), "code-experiments/build/matlab/REVISION")
    write_file(git_version(), "code-experiments/build/matlab/VERSION")

    # make sure that under Windows, run_octave has been run at least once
    # before to provide the necessary octave_coco.bat file
    if 'win32' in sys.platform:
        run('code-experiments/build/matlab',
            ['octave_coco.bat', '--no-gui', 'setup.m'], verbose=_verbosity)
    else:
        run('code-experiments/build/matlab',
            ['octave', '--no-gui', 'setup.m'], verbose=_verbosity)


def run_octave():
    # remove the mex files for a clean compilation first
    print('CLEAN\tmex files from code-experiments/build/matlab/')
    for filename in glob.glob('code-experiments/build/matlab/*.mex*'):
        os.remove(filename)

    # Copy octave-coco.bat to the Octave folder under Windows to allow
    # calling Octave from command line without messing up the system.
    # Note that 'win32' stands for both Windows 32-bit and 64-bit.
    if 'win32' in sys.platform:
        print('SEARCH\tfor Octave folder from C:\\ (can take some time)')
        lookfor = 'octave.bat'
        for root, dirs, files in os.walk('C:\\'):
            if lookfor in files:
                break
        copy_file('code-experiments/build/matlab/octave_coco.bat.in', join(root, 'octave_coco.bat'))

    # amalgamate, copy, and build
    build_octave()
    if 'win32' in sys.platform:
        run('code-experiments/build/matlab',
            ['octave_coco.bat', '--no-gui', 'exampleexperiment.m'], verbose=_verbosity)
    else:
        run('code-experiments/build/matlab',
            ['octave', '--no-gui', 'exampleexperiment.m'], verbose=_verbosity)


def test_octave():
    """ Builds and runs the test in Octave, which is equal to the example experiment """
    build_octave()
    try:
        if 'win32' in sys.platform:
            run('code-experiments/build/matlab',
                ['octave_coco.bat', '--no-gui', 'exampleexperiment.m'],
                verbose=_verbosity)
        else:
            run('code-experiments/build/matlab',
                ['octave', '--no-gui', 'exampleexperiment.m'], verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)


def build_octave_sms():
    """Builds the SMS-EMOA in Octave """
    global RELEASE
    destination_folder = 'code-experiments/examples/bbob-biobj-matlab-smsemoa'
    # amalgamate and copy files
    amalgamate(CORE_FILES + ['code-experiments/src/coco_runtime_c.c'],
               join(destination_folder, 'coco.c'), RELEASE,
               {"COCO_VERSION": git_version(pep440=True)})
    expand_file('code-experiments/src/coco.h', join(destination_folder, 'coco.h'),
                {'COCO_VERSION': git_version(pep440=True)})
    for f in MATLAB_FILES:
        copy_file(join('code-experiments/build/matlab/', f), join(destination_folder, f))
    write_file(git_revision(), join(destination_folder, "REVISION"))
    write_file(git_version(), join(destination_folder, "VERSION"))
    copy_file('code-experiments/build/matlab/cocoCall.c', join(destination_folder, 'cocoCall.c'))
    # compile
    if 'win32' in sys.platform:
        run(destination_folder, ['octave_coco.bat', '--no-gui', 'setup.m'])
    else:
        run(destination_folder, ['octave', '--no-gui', 'setup.m'])


def run_octave_sms():
    """ Builds and runs the SMS-EMOA in Octave

        Note: does not work yet with all Windows/Octave versions.
    """

    print('CLEAN\tmex files from code-experiments/examples/bbob-biobj-matlab-smsemoa/')
    destination_folder = 'code-experiments/examples/bbob-biobj-matlab-smsemoa'
    # remove the mex files for a clean compilation first
    for filename in glob.glob(join(destination_folder, '*.mex*')):
        os.remove(filename)
    # amalgamate, copy, and build
    build_octave_sms()
    wait_for_compilation_to_finish(join(destination_folder, 'paretofront'))
    # run after compilation finished
    if 'win32' in sys.platform:
        run(destination_folder,
            ['octave_coco.bat', '-nogui', 'run_smsemoa_on_bbob_biobj.m'],
            verbose=_verbosity)
    else:
        run(destination_folder,
            ['octave', '-nogui', 'run_smsemoa_on_bbob_biobj.m'],
            verbose=_verbosity)

################################################################################
## Java
def build_java_deps(build_dir):
    """ Builds the example experiment in Java """
    global RELEASE
    build_path = join('code-experiments/build', build_dir)
    amalgamate(CORE_FILES + ['code-experiments/src/coco_runtime_c.c'],
               join(build_path, 'coco.c'), RELEASE,
               {"COCO_VERSION": git_version(pep440=True)})
    expand_file('code-experiments/src/coco.h', join(build_path, 'coco.h'),
                {'COCO_VERSION': git_version(pep440=True)})
    write_file(git_revision(), "code-experiments/build/java/REVISION")
    write_file(git_version(), "code-experiments/build/java/VERSION")
    run(build_path, ['javac', 'CocoJNI.java'], verbose=_verbosity)
    run(build_path, ['javah', 'CocoJNI'], verbose=_verbosity)

    # Finds the path to the headers jni.h and jni_md.h (platform-dependent)
    # and compiles the CocoJNI library (compiler-dependent). So far, only
    # the following cases are covered:

    # 1. Windows with Cygwin (both 64-bit)
    # Note that 'win32' stands for both Windows 32-bit and 64-bit.
    # Since platform 'cygwin' does not work as expected, we need to look for it in the PATH.
    if ('win32' in sys.platform) and ('cygwin' in os.environ['PATH']):
        jdkpath = check_output(['where', 'javac'], stderr=STDOUT,
                               env=os.environ, universal_newlines=True)
        jdkpath1 = jdkpath.split("bin")[0] + 'include'
        jdkpath2 = jdkpath1 + '\\win32'

        if '64' in platform.machine():
            run(build_path, ['x86_64-w64-mingw32-gcc', '-I', jdkpath1, '-I',
                                                jdkpath2, '-shared', '-o', 'CocoJNI.dll',
                                                'CocoJNI.c'], verbose=_verbosity)

            # 2. Windows with Cygwin (both 32-bit)
        elif '32' in platform.machine() or 'x86' in platform.machine():
            run(build_path, ['i686-w64-mingw32-gcc', '-Wl,--kill-at', '-I',
                                                jdkpath1, '-I', jdkpath2, '-shared', '-o',
                                                'CocoJNI.dll', 'CocoJNI.c'], verbose=_verbosity)

    # 3. Windows without Cygwin
    elif ('win32' in sys.platform) and ('cygwin' not in os.environ['PATH']):
        jdkpath = check_output(['where', 'javac'], stderr=STDOUT,
                               env=os.environ, universal_newlines=True)
        jdkpath1 = jdkpath.split("bin")[0] + 'include'
        jdkpath2 = jdkpath1 + '\\win32'
        run(build_path,
            ['gcc', '-Wl,--kill-at', '-I', jdkpath1, '-I', jdkpath2,
             '-shared', '-o', 'CocoJNI.dll', 'CocoJNI.c'],
            verbose=_verbosity)

    # 4. Linux
    elif 'linux' in sys.platform:
        jdkpath = check_output(['locate', 'jni.h'], stderr=STDOUT,
                               env=os.environ, universal_newlines=True)
        jdkpath1 = jdkpath.split("jni.h")[0]
        jdkpath2 = jdkpath1 + '/linux'
        run(build_path,
            ['gcc', '-I', jdkpath1, '-I', jdkpath2, '-c', 'CocoJNI.c'],
            verbose=_verbosity)
        run(build_path,
            ['gcc', '-I', jdkpath1, '-I', jdkpath2, '-o',
             'libCocoJNI.so', '-fPIC', '-shared', 'CocoJNI.c'],
            verbose=_verbosity)

    # 5. Mac
    elif 'darwin' in sys.platform:
        jdkversion = check_output(['javac', '-version'], stderr=STDOUT,
                                  env=os.environ, universal_newlines=True)
        jdkversion = jdkversion.split()[1]
        jdkpath = '/System/Library/Frameworks/JavaVM.framework/Headers'
        jdkpath1 = ('/Library/Java/JavaVirtualMachines/jdk' +
                    jdkversion + '.jdk/Contents/Home/include')
        jdkpath2 = jdkpath1 + '/darwin'
        run(build_path,
            ['gcc', '-I', jdkpath, '-I', jdkpath1, '-I', jdkpath2, '-c', 'CocoJNI.c'],
            verbose=_verbosity)
        run(build_path,
            ['gcc', '-dynamiclib', '-o', 'libCocoJNI.jnilib', 'CocoJNI.o'],
            verbose=_verbosity)

    run(build_path, ['javac', 'Problem.java'], verbose=_verbosity)
    run(build_path, ['javac', 'Benchmark.java'], verbose=_verbosity)
    run(build_path, ['javac', 'Observer.java'], verbose=_verbosity)
    run(build_path, ['javac', 'Suite.java'], verbose=_verbosity)
    run(build_path, ['javac', 'Timing.java'], verbose=_verbosity)


def build_java():
    build_java_deps('java')
    run('code-experiments/build/java', ['javac', 'ExampleExperiment.java'], verbose=_verbosity)


def run_java():
    """ Builds and runs the example experiment in Java """
    build_java()
    try:
        run('code-experiments/build/java',
            ['java', '-Djava.library.path=.', 'ExampleExperiment'],
            verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)


def test_java():
    """ Builds and runs the test in Java, which is equal to the example experiment """
    build_java()
    try:
        run('code-experiments/build/java',
            ['java', '-Djava.library.path=.', 'ExampleExperiment'],
            verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)

def build_scala():
    files = [
        'CocoJNI.c',
        'CocoJNI.java',
        'Benchmark.java',
        'Observer.java',
        'Problem.java',
        'Suite.java',
        'Timing.java',
    ]
    build_dir = 'code-experiments/build'
    java_dir, scala_dir = join(build_dir, 'java'), join(build_dir, 'scala')
    for f in files:
        copy_file(join(java_dir, f), join(scala_dir, f))
    build_java_deps('scala')
    run(scala_dir, ['scalac', 'ExampleExperiment.scala'], verbose=_verbosity)


def run_scala():
    """ Builds and runs the example experiment in Java """
    build_scala()
    try:
        run('code-experiments/build/scala',
            ['scala', '-Djava.library.path=.', 'ExampleExperiment'],
            verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)


def test_scala():
    """ Builds and runs the test in Java, which is equal to the example experiment """
    build_scala()
    try:
        run('code-experiments/build/scala',
            ['scala', '-Djava.library.path=.', 'ExampleExperiment'],
            verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)

################################################################################
## Post processing
def test_postprocessing(all_tests=False, package_install_option=[]):
    install_postprocessing(package_install_option = package_install_option)
    try:
        if all_tests:
            # run example experiment to have a recent data set to postprocess:
            build_python(package_install_option=package_install_option)
            python('code-experiments/build/python/', ['-c', '''
from __future__ import print_function
try:
    import example_experiment as ee
except Exception as e:
    print(e)
ee.SOLVER = ee.random_search  # which is default anyway
for ee.suite_name, ee.observer_options['result_folder'] in [
        ["bbob-biobj", "RS-bi"],  # use a short path for Jenkins
        ["bbob", "RS-bb"],
        ["bbob-constrained", "RS-co"]
    ]:
    print("  suite %s" % ee.suite_name, end=' ')  # these prints are swallowed
    if ee.suite_name in ee.cocoex.known_suite_names:
        print("testing into folder %s" % ee.observer_options['result_folder'])
        ee.main()
    else:
        print("is not known")
                '''], verbose=_verbosity)
            # now run all tests
            python('code-postprocessing/cocopp',
                   ['test.py', 'all', sys.executable], verbose=_verbosity)
        else:
            python('code-postprocessing/cocopp', ['test.py', sys.executable],
                   verbose=_verbosity)

        # also run the doctests in aRTAplots/generate_aRTA_plot.py:
        python('code-postprocessing/aRTAplots', ['generate_aRTA_plot.py'], verbose=_verbosity)
    except subprocess.CalledProcessError:
        sys.exit(-1)
    finally:
        # always remove folder of previously run experiments:
        exdata_folder = 'code-experiments/build/python/exdata/'
        if os.path.exists(exdata_folder):
            shutil.rmtree(exdata_folder)


def verify_postprocessing(package_install_option = []):
    install_postprocessing(package_install_option = package_install_option)
    # This is not affected by the _verbosity value. Verbose should always be True.
    python('code-postprocessing/cocopp', ['preparehtml.py', '-v'], verbose=True)


################################################################################
## Pre-processing
def install_preprocessing(package_install_option = []):
    global RELEASE
    expand_file(join('code-preprocessing/archive-update', 'setup.py.in'),
                join('code-preprocessing/archive-update', 'setup.py'),
                {'COCO_VERSION': git_version(pep440=True)})
    build_python(package_install_option = package_install_option)
    amalgamate(CORE_FILES + ['code-experiments/src/coco_runtime_c.c'],
               'code-preprocessing/archive-update/interface/coco.c', RELEASE,
               {"COCO_VERSION": git_version(pep440=True)})
    expand_file('code-experiments/src/coco.h', 'code-preprocessing/archive-update/interface/coco.h',
                {'COCO_VERSION': git_version(pep440=True)})
    python('code-preprocessing/archive-update',
           ['setup.py', 'install'] + package_install_option,
           verbose=_verbosity, custom_exception_handler=install_error)


def test_preprocessing(package_install_option = []):
    install_preprocessing(package_install_option = package_install_option)
    python('code-preprocessing/archive-update', ['-m', 'pytest'], verbose=_verbosity)
    python('code-preprocessing/log-reconstruction', ['-m', 'pytest'], verbose=_verbosity)

################################################################################
## Global
def build(package_install_option = []):
    builders = [
        build_c,
        # build_matlab,
        build_python(package_install_option = package_install_option),
        build_java,
    ]
    for builder in builders:
        try:
            builder()
        except:
            failed = str(builder)
            print("============")
            print('   ERROR: %s failed, call "./do.py %s" individually'
                  % (failed, failed[failed.find('build_'):].split()[0]) +
                  ' for a more detailed error report')
            print("============")


def run_all(package_install_option = []):
    run_c()
    run_java()
    run_python(package_install_option = package_install_option)


def test(package_install_option = []):
    test_c()
    test_java()
    test_python(package_install_option = package_install_option)


def verbose(args):
    """Calls main(args) in verbose mode for additional output"""
    global _verbosity
    _verbosity = True
    main(args)
    _verbosity = False

def quiet(args):
    """Calls main(args) in quiet mode for less output during c builds"""
    global _build_verbosity
    _build_verbosity = False
    main(args)
    _build_verbosity = True

def silent(args):
    """calls `main(args)` with redirected output to keep the console clean"""
    # redirect stdout and call main
    filename = '_check_output'
    raised = None
    stdout = sys.stdout

    with open(filename, 'w') as out:
        sys.stdout = out
        try:
            main(args)
        except BaseException as raised:
            pass
    sys.stdout = stdout

    # check whether an error occured
    error = False
    for line in open(filename, 'r').readlines():
        if line.startswith('ERR') or not line[0] in 'ABCDEFGHIJKLMNOPQRSTUVWXYZ':
            error = True
            break
    if error:
        for line in open(filename, 'r').readlines():
            print(line, end="")
    if raised:
        raise raised


def help():
    print("""COCO framework bootstrap tool. Version %s

Usage: do.py <command> <arguments>

If you want to get going as quickly as possible do once

   python do.py run-<your-language>

and

    python do.py install-postprocessing

and you are all set.

Available commands for users:

  build-c                 - Build C module
  build-java              - Build Java module
  build-matlab            - Build Matlab module
  build-matlab-sms        - Build SMS-EMOA example in Matlab
  build-octave            - Build Matlab module in Octave
  build-python            - Build Python modules (see NOTE below)
  install-postprocessing  - Install postprocessing (see NOTE below)

  run-c                   - Build and run example experiment in C
  run-java                - Build and run example experiment in Java
  run-matlab              - Build and run example experiment in MATLAB
  run-matlab-sms          - Build and run SMS-EMOA on bbob-biobj suite in
                            MATLAB
  run-octave              - Build and run example experiment in Octave
  run-python              - Build and install COCO module and then run the
                            example experiment in Python. The optional
                            parameter "and-test" also runs the tests of
                            `coco_test.py` (see NOTE below)

Available commands for developers:

  build                   - Build C, Java and Python modules (see NOTE below)
  run                     - Run example experiments in C, Java and Python (see
                            NOTE below)
  silent cmd ...          - Calls "do.py cmd ..." and remains silent if no
                            error occurs
  verbose cmd ...         - Calls "do.py cmd ..." and shows more output
  test                    - Test C, Java and Python modules (see NOTE below)

  run-sandbox-python      - Run a Python script with installed COCO module
                            Takes a single argument(name of Python script file)

  test-c                  - Build and run unit tests, integration tests
                            and an example experiment test in C
  test-c-unit             - Build and run unit tests in C
  test-c-integration      - Build and run integration tests in C
  test-c-example          - Build and run an example experiment test in C
  test-java               - Build and run a test in Java
  test-python             - Build and run minimal test of Python module
  test-octave             - Build and run example experiment in Octave
  test-postprocessing     - Runs some of the post-processing tests (see NOTE
                            below)
  test-postprocessing-all - Runs all of the post-processing tests [needs access
                            to the internet] (see NOTE below)
  test-suites             - Runs regression test on all benchmark suites
  verify-postprocessing   - Checks if the generated html is up-to-date (see
                            NOTE below)
  leak-check              - Check for memory leaks in C
  
  install-preprocessing   - Install preprocessing (user-locally) (see NOTE
                            below)
  test-preprocessing      - Runs preprocessing tests [needs access to the
                            internet] (see NOTE below)
  
NOTE: These commands install Python packages to the global site packages by
      by default. This behavior can be modified by providing one of the
      following arguments.
  
       install-user       - Installs under the user directory
       install-home=<dir> - Installs under the specified home directory

To build a release version which does not include debugging information in the
amalgamations set the environment variable COCO_RELEASE to 'true'.
""" % git_version(pep440=True))


def main(args):
    if len(args) < 1:
        help()
        sys.exit(0)
    cmd = args[0].replace('_', '-').lower()
    also_test_python = False
    package_install_option = []
    for arg in args[1:]:
        if arg == 'and-test':
            also_test_python = True
        elif arg == 'install-user':
            package_install_option = ['--user']
        elif arg[:13] == 'install-home=':
            package_install_option = ['--home=' + arg[13:]]
    if cmd == 'build': build(package_install_option = package_install_option)
    elif cmd == 'run': run_all(package_install_option = package_install_option)
    elif cmd == 'test': test(package_install_option = package_install_option)
    elif cmd == 'build-c': build_c()
    elif cmd == 'build-java': build_java()
    elif cmd == 'build-matlab': build_matlab()
    elif cmd == 'build-matlab-sms': build_matlab_sms()
    elif cmd == 'build-octave': build_octave()
    elif cmd == 'build-octave-sms': build_octave_sms()
    elif cmd == 'build-python': build_python(package_install_option = package_install_option)
    elif cmd == 'install-postprocessing': install_postprocessing(package_install_option = package_install_option)
    elif cmd == 'run-c': run_c()
    elif cmd == 'run-java': run_java()
    elif cmd == 'run-scala': run_scala()
    elif cmd == 'run-matlab': run_matlab()
    elif cmd == 'run-matlab-sms': run_matlab_sms()
    elif cmd == 'run-octave': run_octave()
    elif cmd == 'run-octave-sms': run_octave_sms()
    elif cmd == 'run-python': run_python(also_test_python, package_install_option = package_install_option)
    elif cmd == 'quiet': quiet(args[1:])
    elif cmd == 'silent': silent(args[1:])
    elif cmd == 'verbose': verbose(args[1:])
    elif cmd == 'test-c': test_c()
    elif cmd == 'test-c-unit': test_c_unit()
    elif cmd == 'test-c-integration': test_c_integration()
    elif cmd == 'test-c-example': test_c_example()
    elif cmd == 'test-java': test_java()
    elif cmd == 'test-scala': test_scala()
    elif cmd == 'test-python': test_python()
    elif cmd == 'test-octave': test_octave()
    elif cmd == 'test-postprocessing': test_postprocessing(all_tests = False, package_install_option = package_install_option)
    elif cmd == 'test-postprocessing-all': test_postprocessing(all_tests = True, package_install_option = package_install_option)
    elif cmd == 'test-suites': test_suites(args[1:])
    elif cmd == 'verify-postprocessing': verify_postprocessing(package_install_option = package_install_option)
    elif cmd == 'leak-check': leak_check()
    elif cmd == 'install-preprocessing': install_preprocessing(package_install_option = package_install_option)
    elif cmd == 'test-preprocessing': test_preprocessing(package_install_option = package_install_option)
    else: help()


if __name__ == '__main__':
    RELEASE = os.getenv('COCO_RELEASE', 'false') == 'true'
    main(sys.argv[1:])
