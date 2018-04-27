numbbo/coco: Comparing Continuous Optimizers
============================================

[![CircleCI](https://circleci.com/gh/numbbo/coco/tree/master.svg?style=shield)](https://circleci.com/gh/numbbo/coco/tree/master)

[This code](https://github.com/numbbo/coco) reimplements the original Comparing Continous Optimizer platform, 
now rewritten fully in `ANSI C` with other languages calling the `C` code. As the name suggests,
the code provides a platform to benchmark and compare continuous optimizers, AKA non-linear 
solvers for numerical optimization. Languages currently available are 

  - `C/C++`
  - `Java`
  - `MATLAB/Octave`
  - `Python`
  - `Scala`

Contributions to link further languages (including a better
example in `C++`) are more than welcome.

For more information, 
- read our [benchmarking guidelines introduction](http://numbbo.github.io/coco-doc/)
- read the [COCO experimental setup](http://numbbo.github.io/coco-doc/experimental-setup) description
- see the [`bbob-biobj` and `bbob-biobj-ext` COCO multi-objective functions testbed](http://numbbo.github.io/coco-doc/bbob-biobj/functions) documentation and the [specificities of the performance assessment for the bi-objective testbeds](http://numbbo.github.io/coco-doc/bbob-biobj/perf-assessment).
- consult the [BBOB workshops series](http://numbbo.github.io/workshops),
- consider to [register here](http://numbbo.github.io/register) for news, 
- see the [previous COCO home page here](http://coco.gforge.inria.fr/) and 
- see the [links below](#Links) to learn more about the ideas behind CoCO.

Requirements  <a name="Requirements"></a>
------------
1. For a machine running experiments 
  - A `C` compiler, such as gcc
  - Python >=2.6 with `setuptools` installed
  - optional: `git`
2. For a machine running the post-processing
  - Python >=2.6 with `setuptools`, `numpy`, `scipy`, `matplotlib`, and `six` installed.
    We recommend to install the [Anaconda Python 2.7 library](https://www.continuum.io/downloads). 

For Ubuntu 16.04+, all the requirements can be installed using the following command:
```
apt-get install build-essential python-dev python-numpy python-matplotlib \
                python-scipy python-six python-setuptools
```

### Windows Specifics
Under Windows, two alternative compile toolchains can be used: 

1. [Cygwin](https://www.cygwin.com/) which comes with gcc and make, available in 32- and 64-bit versions.  
2. MinGW's gcc (http://www.mingw.org/), which only comes in 32-bit, but also runs on 64-bit machines. 

For using `git` under Windows (optional), we recommend installing [TortoiseGit](https://tortoisegit.org/).

### Programming Language Specifics  <a name="Language-Specifics"></a>
_Additional_ requirements for running an algorithm in a specific language.

* **C**: `make`, such as GNU make (when using [GNU make for Windows](http://gnuwin32.sourceforge.net/packages/make.htm), make sure that your ``CC`` environment variable is set to `gcc` by potentially typing `set CC=gcc` if you see an error). 
* **Java**: <a name="Java-Requirements"></a> `gcc` and any Java Development Kit (JDK), such that `javac` and `javah` are accessible
  (i.e. in the system path).
* **MATLAB**: at least MATLAB 2008, for details, see [here](./code-experiments/build/matlab/README.md)
* **Python on Windows with MinGW**: Python 2.7 and the Microsoft compiler package for Python 2.7 
  containing VC9, available [here](https://www.microsoft.com/en-us/download/details.aspx?id=44266). 
  These are necessary to build the C extensions for the Python `cocoex` module for Windows. 
  The package contains 32-bit and 64-bit compilers and the Windows SDK headers.
* **Python on Linux**: `python-dev` must be installed to compile/install the `cocoex` module.
* **Octave**: Octave 4.0.0 or later. On operating systems other than Windows, earlier versions might work.
  Under Linux the package `liboctave-dev` might be necessary. 
* **Scala**: `scala` and all [Java requirements](#Java-Requirements).

### Guaranties (None)
We tested the framework on Mac OSX, Ubuntu linux, Fedora linux, and Windows (XP,
7, 10) in various combinations of 32-bit and 64-bit compilers, python versions
etc. Naturally, we cannot guarantee that the framework runs on any combination
of operating system and software installed. In case you experience some incompatibilies,
check out the [_Known Issues / Trouble Shooting_ Section](#Known-Issues) below. 
Otherwise we will be happy if you can document them in detail on the 
[issue tracker](https://github.com/numbbo/coco/issues). 


Getting Started <a name="Getting-Started"></a>
---------------
0. Check out the [_Requirements_](#Requirements) above.

1. **Download** the COCO framework code from github, 

  - either by clicking the [Download ZIP button](https://github.com/numbbo/coco/archive/master.zip) 
    and unzip the `zip` file, 
  - or by typing `git clone https://github.com/numbbo/coco.git`. This way 
    allows to remain up-to-date easily (but needs `git` to be installed). After 
    cloning, `git pull` keeps the code up-to-date with the latest release. 

  The record of official releases can 
  be found [here](https://github.com/numbbo/coco/releases). The latest release corresponds 
  to the [master branch](https://github.com/numbbo/coco/tree/master) as linked above. 

2. In a system shell, **`cd` into** the `coco` or `coco-<version>` folder (framework root), 
  where the file `do.py` can be found. Type, i.e. **execute**, one of the following commands once
  ```
    python do.py run-c
    python do.py run-java
    python do.py run-matlab
    python do.py run-octave
    python do.py run-python
  ```  
  depending on which language shall be used to run the experiments. `run-*` will build the 
  respective code and run the example experiment once. The build result and the example
  experiment code can be found under `code-experiments/build/<language>` (`<language>=matlab` 
  for Octave). `python do.py` lists all available commands. 
  
3. On the computer where experiment data shall be post-processed, run

  ```
    python do.py install-postprocessing
  ```
  to (user-locally) install the post-processing. From here on, `do.py` has done
  its job and is only needed again for updating the builds to a new release.
  
  
4. **Copy** the folder `code-experiments/build/YOUR-FAVORITE-LANGUAGE` and
  its content to another location. In Python it is sufficient to copy the 
  file `example_experiment.py`. Run the example experiment (it already is
  compiled). As the details vary, see the respective read-me's 
  and/or example experiment files:

  - `C` [read me](./code-experiments/build/c/README.md) 
    and [example experiment](./code-experiments/build/c/example_experiment.c)
  - `Java` [read me](./code-experiments/build/java/README.md)
    and [example experiment](./code-experiments/build/java/ExampleExperiment.java)
  - `Matlab/Octave` [read me](./code-experiments/build/matlab/README.md)
    and [example experiment](./code-experiments/build/matlab/exampleexperiment.m) 
  - `Python` [read me](./code-experiments/build/python/README.md)
    and [example experiment](./code-experiments/build/python/example_experiment.py)

  If the example experiment runs, **connect** your favorite algorithm
  to Coco: replace the call to the random search optimizer in the
  example experiment file by a call to your algorithm (see above).
  **Update** the output `result_folder`, the `algorithm_name` and `algorithm_info` 
  of the observer options in the example experiment file.

  Another entry point for your own experiments can be the `code-experiments/examples`
  folder. 

5. Now you can **run** your favorite algorithm on the `bbob` suite (for 
  single-objective algorithms) or on the `bbob-biobj` and `bbob-biobj-ext`
  suites (for multi-objective algorithms). Output is automatically generated in the 
  specified data `result_folder`. By now, more suites might be available, see below. 

  <a name="Getting-Started-pp"></a>
6.  **Postprocess** the data from the results folder by typing

  ```
    python -m cocopp [-o OUTPUT_FOLDERNAME] YOURDATAFOLDER [MORE_DATAFOLDERS]
  ```

  Any subfolder in the folder arguments will be searched for logged data. That is, 
  experiments from different batches can be in different folders collected under a 
  single "root"  `YOURDATAFOLDER` folder. We can also compare more than one algorithm 
  by specifying several data result folders generated by different algorithms.
  Note that we provide many algorithm data sets by default. For the 
  `bbob` test suite, the data can be found at http://coco.gforge.inria.fr/doku.php?id=algorithms
  and for the `bbob-biobj` test suite, the data can be found at 
  http://coco.gforge.inria.fr/doku.php?id=algorithms-biobj.
  
  A folder, `ppdata` by default, will be generated, which contains all output from 
  the post-processing, including an `index.html` file, useful as main entry point to 
  explore the result with a browser. Data might be overwritten, it is therefore 
  useful to change the output folder name with the `-o OUTPUT_FOLDERNAME` 
  option.

  A summary pdf can be produced via LaTeX. The corresponding templates can be found 
  in the `code-postprocessing/latex-templates` folder.  Basic html output is also 
  available in the result folder of the postprocessing (file `templateBBOBarticle.html`).

7. Once your algorithm runs well, **increase the budget** in your experiment
  script, if necessary implement randomized independent restarts, and follow 
  the above steps successively until you are happy.
  
8. The experiments can be **parallelized** with any re-distribution of single
  problem instances to batches (see
  [`example_experiment.py`](./code-experiments/build/python/example_experiment.py#L235) 
  for an example). Each batch must write in a different target folder (this
  should happen automatically). Results of each batch must be kept under their
  separate folder as is. These folders then must be moved/copied into a single
  folder which becomes the input folder to the post-processing. (The
  post-processing searches in all subfolders and subsub... for `.info` files
  to begin with. The folder structure of a single sub-experiment must not be
  changed, as the `.info` file relies on it to find the data files.)
  
  
If you detect bugs or other issues, please let us know by opening an issue in
our issue tracker at https://github.com/numbbo/coco/issues.

## Description by Folder

* the `do.py` file in the root folder is a tool to build the entire
  distribution. `do.py` is a neat and simplifying replacement for make. It has
  switches for just building some languages etc, type
  ```
    python do.py
  ```
  to see a list of all available commandes. 

* the code-experiments/build folder is to a large extend the output folder of
  the `./do.py build` command.
   - the `exampleexperiment.???` files in the build folder are the entry points to
     understand the usage of the code (as end-user). They are supposed to
     actually be executable (in case, after compilation, which should be taken
     care of by `do.py` and/or `make`) and run typically random search on (some of)
     the provided benchmark suites.

* documentation and examples might not be too meaningful for the time being,
  even though code-experiments/documentation/onion.py describes a (heavily) used
  design pattern (namely: inheritance) in a comparatively understandable way
  (though the implementation in C naturally looks somewhat different). Section 
  [Links and Documentation](https://github.com/numbbo/coco#links-and-documentation-)
  provides a list of pointers.

* the code-experiments/src folder is where most of the important/interesting
  things happen. Many files provide comparatively decent documentation at the
  moment which are translated via doxygen into a more readable web page at
  http://numbbo.github.io/coco-doc/C/. Generally:
  - [coco.h](./code-experiments/src/coco.h) is the public interface, in particular
    as used in the example_experiment.c file
  - coco_internal.h provides the type definition of coco_problem_t
  - coco_suite.c is code that deals with an entire benchmark suite (i.e. a set of
    functions, eg. sweeping through them etc...)
  - coco_generics.c is somewhat generic code, e.g. defining a function call via
    coco_evaluate_function etc
  - coco_problem.c is the implementation of the coco_problem_t type/object
    (allocation etc).
  - observer / logger files implement data logging (as wrappers around a coco
    problem inheriting thereby all properties of a coco problem)
  - most other files implement more or less what they say, e.g. the actual
    benchmark functions, transformations, benchmark suites, etc.
  - currently, four benchmark suites and corresponding logging facilities are
    implemented:
    * `bbob`: standard single-objective BBOB benchmark suite with 24 noiseless,
      scalable test functions
    * `bbob-biobj`: a bi-objective benchmark suite, combining 10 selected
      functions from the bbob suite, resulting in 55 noiseless functions
    * `bbob-biobj-ext`: an extended version of the bi-objective `bbob-biobj`
       benchmark suite, adding `bbob` function combinations from the same
       `bbob` function groups, resulting in 92 noiseless functions overall
    * `toy`: a simple, probably easier-to-understand example for reading and testing

* code-experiments/tools are a few meta-tools, mainly the amalgamate.py to merge all
  the C code into one file

* code-experiments/test contains unit- and integration-tests, mainly for internal use

* code-postprocessing/cocopp contains the postprocessing code, written in
  python, with which algorithm data sets can be read in and the performance of
  the algorithms can be displayed in terms of data profiles, aRT vs. dimension
  plots, or simple tables.

* code-postprocessing/helper-scripts contains additional, independent python scripts 
  that are not part of the cocopp module but that might use it.
  
* code-postprocessing/latex-templates contains LaTeX templates for displaying
  algorithm performances in publisher-conform PDFs for the GECCO
  conference.

* code-preprocessing/archive-update/ contains internal code for combining
  the archives of algorithms to create/update the hypervolume
  reference values for the `bbob-biobj` test suite

* code-preprocessing/log-reconstruction/ contains internal code for reconstructing
  output of the `bbob-biobj` logger from archive files (needed when the hypervolume
  reference values are updated)

* docs should contain an updated version of the documentation, see above.

* howtos contains a few text files with internal howtos.


Known Issues / Trouble-Shooting <a name="Known-Issues"></a>
-------------------------------
### Java
#### `javah` call fails
If you see something like this when running `python do.py run-java` or `build-java`
under Linux
```
COPY    code-experiments/src/coco.h -> code-experiments/build/java/coco.h
WRITE   code-experiments/build/java/REVISION
WRITE   code-experiments/build/java/VERSION
RUN     javac CocoJNI.java in code-experiments/build/java
RUN     javah CocoJNI in code-experiments/build/java
Traceback (most recent call last):
  File "do.py", line 590, in <module>
    main(sys.argv[1:])
  File "do.py", line 563, in main
    elif cmd == 'build-java': build_java()
  File "do.py", line 437, in build_java
    env = os.environ, universal_newlines = True)
  File "/..../code-experiments/tools/cocoutils.py", line 34, in check_output
    raise error
subprocess.CalledProcessError: Command '['locate', 'jni.h']' returned non-zero exit status 1
```
it means `javah` is either not installed (see above) or cannot be found in the system
path, see [this](http://stackoverflow.com/questions/13526701/javah-missing-after-jdk-install-linux)
and possibly [this](https://github.com/numbbo/coco/issues/416) for a solution. 

### Matlab

#### Path to matlab
If you see something like this when running `python do.py build-matlab`
```
AML	['code-experiments/src/coco_generics.c', 'code-experiments/src/coco_random.c', 'code-experiments/src/coco_suite.c', 'code-experiments/src/coco_suites.c', 'code-experiments/src/coco_observer.c', 'code-experiments/src/coco_runtime_c.c'] -> code-experiments/build/matlab/coco.c
COPY	code-experiments/src/coco.h -> code-experiments/build/matlab/coco.h
COPY	code-experiments/src/best_values_hyp.txt -> code-experiments/build/matlab/best_values_hyp.txt
WRITE	code-experiments/build/matlab/REVISION
WRITE	code-experiments/build/matlab/VERSION
RUN	matlab -nodisplay -nosplash -r setup, exit in code-experiments/build/matlab
Traceback (most recent call last):
  File "do.py", line 447, in <module>
    main(sys.argv[1:])
  File "do.py", line 429, in main
    elif cmd == 'build-matlab': build_matlab()
  File "do.py", line 278, in build_matlab
    run('code-experiments/build/matlab', ['matlab', '-nodisplay', '-nosplash', '-r', 'setup, exit'])
  File "/Users/auger/workviasvn/newcoco/numbbo/code-experiments/tools/cocoutils.py", line 68, in run
    universal_newlines=True)
  File "//anaconda/lib/python2.7/subprocess.py", line 566, in check_output
    process = Popen(stdout=PIPE, *popenargs, **kwargs)
  File "//anaconda/lib/python2.7/subprocess.py", line 710, in __init__
    errread, errwrite)
  File "//anaconda/lib/python2.7/subprocess.py", line 1335, in _execute_child
    raise child_exception
OSError: [Errno 2] No such file or directory
```
it might be because your system does not know the `matlab` command. To fix this,
you should edit the file `/etc/paths` and add the path to the `matlab` bin file 
(Linux/Mac) or add the path to the folder where the `matlab.exe` lies to your 
Windows path. For instance, the `etc/paths` should look like something like this
```
/usr/local/bin
/usr/bin
/bin
/usr/sbin
/sbin
/Applications/MATLAB_R2012a.app/bin/
```

#### SMS-EMOA example does not compile under Mac 
With the more complex SMS-EMOA example, the problem is related to the compilation
of the external C++ hypervolume calculation in `hv.cpp`. 

A fix for this issue consists in adding to the files `hv.cpp` and `paretofront.c`
```
#define char16_t UINT16_T
```
just before the line:
```
#include "mex.h"
```

#### Access to mex files denied
If it happens that you get some `Access is denied` errors during
`python do.py build-matlab` or `python do.py run-matlab` like this one
```
C:\Users\dimo\Desktop\numbbo-brockho>python do.py run-matlab
Traceback (most recent call last):
  File "do.py", line 649, in <module>
    main(sys.argv[1:])
  File "do.py", line 630, in main
    elif cmd == 'run-matlab': run_matlab()
  File "do.py", line 312, in run_matlab
    os.remove( filename )
WindowsError: [Error 5] Access is denied: 'code-experiments/build/matlab\\cocoEv
aluateFunction.mexw32'
```
a reason can be that a previously opened Matlab window still has some
file handles open. Simply close all Matlab windows (and all running Matlab
processes if there is any) before to run the `do.py` command again.


### Octave

#### `octave-dev` under Linux
When running 
```
  python do.py run-octave
```
or 
```
  python do.py build-octave
```
and seeing something like
```
   [...]
   compiling cocoCall.c...error: mkoctfile: please install the Debian package "liboctave-dev" to get the mkoctfile command
```
then, unsurprisingly, installing `liboctave-dev` like
```
  sudo apt-get install liboctave-dev
```
should do the job. 


### Python

#### `setuptools` is not installed
If you see something like this
```
$ python do.py run-python  # or build-python
[...]
PYTHON  setup.py install --user in code-experiments/build/python
ERROR: return value=1
Traceback (most recent call last):
 File "setup.py", line 8, in <module>
   import setuptools
ImportError: No module named setuptools

Traceback (most recent call last):
 File "do.py", line 562, in <module>
   main(sys.argv[1:])
 File "do.py", line 539, in main
   elif cmd == 'build-python': build_python()
 File "do.py", line 203, in build_python
   python('code-experiments/build/python', ['setup.py', 'install', '--user'])
 File "/vol2/twagner/numbbo/code-experiments/tools/cocoutils.py", line 92, in p                                         ython
   universal_newlines=True)
 File "/usr/local/lib/python2.7/subprocess.py", line 575, in check_output
   raise CalledProcessError(retcode, cmd, output=output)
subprocess.CalledProcessError: Command '['/usr/local/bin/python', 'setup.py', 'i                                        nstall', '--user']' returned non-zero exit status 1
```
then `setuptools` needs to be installed: 
```
    pip install setuptools
```
or `easy_install setuptools` should do the job. 

#### Compilation During Install of `cocoex` Fails (under Linux) 
If you see something like this:
``` 
$ python do.py run-python  # or build-python
[...]
cython/interface.c -o build/temp.linux-i686-2.6/cython/interface.o
cython/interface.c:4:20: error: Python.h: file not found
cython/interface.c:6:6: error: #error Python headers needed to compile C extensions, please install development version of Python.
error: command 'gcc' failed with exit status 1
```
or
```
$ python do.py run-python  # or build-python
[...]
cython/interface.c -o build/temp.linux-x86_64-2.7/cython/interface.o
cython/interface.c:4:20: fatal error: Python.h: No such file or directory
#include "Python.h"
^
compilation terminated.
error: command 'x86_64-linux-gnu-gcc' failed with exit status 1
```
Under Linux
```
  sudo apt-get install python-dev
```
should do the trick. 

#### Module Update/Install Does Not Propagate
We have observed a case where the update of the `cocoex` Python module seemed to have no 
effect. In this case it has been successful to remove all previously installed versions, 
see [here](https://github.com/numbbo/coco/issues/586) for a few more details. 


### Post-Processing

#### Too long paths for postprocessing
It can happen that the postprocessing fails due to too long paths to the algorithm data.
Unfortunately, the error you get in this case does not indicate directly to the problem
but only tells that a certain file could not be read. Please try to shorten the
folder names in such a case.


#### Font issues in PDFs
We have occasionally observed some font issues in the pdfs, produced by the postprocessing
of COCO (see also issue [#1335](https://github.com/numbbo/coco/issues/1335)). Changing to 
another `matplotlib` version solved the issue at least temporarily.

#### BibTeX under Mac
Under the Mac operating system, `bibtex` seems to be messed up a bit with respect to
absolute and relative paths which causes problems with the test of the postprocessing
via `python do.py test-postprocessing`. Note that there is typically nothing to fix if 
you compile the LaTeX templates "by hand" or via your LaTeX IDE. But to make the  
`python do.py test-postprocessing` work, you will have to add a line with
`openout_any = a` to your `texmf.cnf` file in the local TeX path. Type 
`kpsewhich texmf.cnf` to find out where this file actually is.

#### Postprocessing not installable
If for some reason, your python installation is corrupted and running
`python do.py install-postprocessing` crashes with an error message like
```
[...]
    safe = scan_module(egg_dir, base, name, stubs) and safe
  File "C:\Users\dimo\Anaconda2\lib\site-packages\setuptools\command\bdist_egg.py", line 392, in sca
n_module
    code = marshal.load(f)
EOFError: EOF read where object expected
[...]
```
try adding `zip_safe=False` to the `setup.py.in` file in the `code-postprocessing`
folder. More details can be found in the issue [#1373](https://github.com/numbbo/coco/issues/1373).

#### Algorithm appears twice in the figures
Earlier versions of `cocopp` have written extracted data to a folder named `_extracted_...`. 
If the post-processing is invoked with a `*` argument, these folders become an argument and 
are displayed (most likely additionally to the original algorithm data folder). Solution: 
remove the `_extracted_...` folders _and_ use the latest version of the post-processing
module `cocopp` (since release 2.1.1).


Details
-------
- The C code features an object oriented implementation, where the
  `coco_problem_t` is the most central data structure / object. `coco.h`,
  `example_experiment.c` and `coco_internal.h` are probably the best pointers to
  start __to investigate the code__ (but see also 
  [here](https://numbbo.github.io/coco-doc/C/annotated.html)). `coco_problem_t`
  defines a benchmark function instance (in a given dimension), and is called
  via `coco_evaluate_function`.

- Building, running, and testing of the code is done by merging/amalgamation of
  all C-code into a single C file, `coco.c`, and `coco.h`. (by calling `do.py`,
  see above). Like this it becomes very simple to include/use the code in
  different projects.

- [Cython](http://docs.cython.org/src/quickstart/install.html) is used to
  compile the C to Python interface in `build/python/interface.pyx`. The Python
  module installation file `setup.py` uses the compiled `interface.c`, if
  `interface.pyx` has not changed. For this reason, Cython is not a requirement
  for the end-user.

- We continuously test the code through the open source automation server
  Jenkins on one ubuntu 12.04 machine, one OSX 10.9 machine, and two 32-bit
  Windows 7 machines (one with and one without cygwin).

Citation
--------
You may cite this work in a scientific context as

N. Hansen, A. Auger, O. Mersmann, T. Tušar, D. Brockhoff. [COCO: A Platform for Comparing Continuous Optimizers in a Black-Box Setting](http://numbbo.github.io/coco-doc/), _ArXiv e-prints_, [arXiv:1603.08785](http://arxiv.org/abs/1603.08785), 2016.

```
@ARTICLE{hansen2016cocoplat, 
  author = {Hansen, N. and Auger, A. and Mersmann, O. and 
             Tu{\v s}ar, T. and Brockhoff, D.},
  title = {{COCO}: A Platform for Comparing Continuous Optimizers 
             in a Black-Box Setting},
  journal = {ArXiv e-prints},
  volume = {arXiv:1603.08785},
  year = 2016
}
```

Links and Documentation <a name="Links"></a>
-----------------------
* The [_BBOB workshop series_](http://numbbo.github.io/workshops), which uses the
  NumBBO/Coco framework extensively, can be tracked
  [here](http://numbbo.github.io/workshops "BBOB Workshops")
* Data sets from previous experiments for many algorithms are available
  at http://coco.gforge.inria.fr/doku.php?id=algorithms-bbob for the `bbob` test suite, 
  at http://coco.gforge.inria.fr/doku.php?id=algorithms-bbob-noisy for the `bbob-noisy`
  test suite and at http://coco.gforge.inria.fr/doku.php?id=algorithms-bbob-biobj
  for the `bbob-biobj` test suite.
* Postprocessed data for each year in which a BBOB workshop was taking place can be
  found at http://coco.gforge.inria.fr/ppdata-archive
* Stay informed about the BBOB workshop series and releases of the NumBBO/Coco software 
  by registering at http://coco.gforge.inria.fr/register
* Read about the basic principles behind the Coco platform at http://numbbo.github.io/coco-doc/.
* For details on the experimental set-up to carry out the benchmarking
  please refer to http://numbbo.github.io/coco-doc/experimental-setup/.
* More detailed documentation of the existing benchmark suites can be found here:
  - for the **`bbob`** problem suite at http://coco.lri.fr/downloads/download15.03/bbobdocfunctions.pdf 
    with the experimental setup at http://coco.lri.fr/downloads/download15.03/bbobdocexperiment.pdf
  - for the **`bbob-biobj`** and **`bbob-biobj-ext`** problem suites
    at http://numbbo.github.io/coco-doc/bbob-biobj/functions
* Online documentation of the NumBBO/Coco API (i.e. for the ANSI C code) is available at 
  http://numbbo.github.io/coco-doc/C
* More detailed documentation on how the performance assessement is done can 
  be found at http://numbbo.github.io/coco-doc/perf-assessment/.
* Downloading this repository 
  - via the above green "Clone or Download" button or 
  - by typing `git clone https://github.com/numbbo/coco.git` or 
  - via https://github.com/numbbo/coco/archive/master.zip in your browser
  
Comprehensive List of Documentations <a name="Documentations"></a>
--------------------------------------------
* General introduction: http://numbbo.github.io/coco-doc
* Experimental setup: http://numbbo.github.io/coco-doc/experimental-setup/
* Testbeds
  - bbob: http://coco.lri.fr/downloads/download15.03/bbobdocfunctions.pdf ^1
  - bbob-biobj: http://numbbo.github.io/coco-doc/bbob-biobj/functions/
  - bbob-biobj-ext: http://numbbo.github.io/coco-doc/bbob-biobj/functions/
  - bbob-noisy (only in old code basis): http://coco.lri.fr/downloads/download15.03/bbobdocnoisyfunctions.pdf
  - bbob-constrained (in progress): http://numbbo.github.io/coco-doc/bbob-constrained/functions/
  - bbob-largescale (in progress): http://numbbo.github.io/coco-doc/bbob-largescale/functions/

* Performance assessment: http://numbbo.github.io/coco-doc/perf-assessment/
* Performance assessment for biobjective testbeds: http://numbbo.github.io/coco-doc/bbob-biobj/perf-assessment/

* APIs
  - ``C`` experiments code: http://numbbo.github.io/coco-doc/C
  - Python experiments code `cocoex`: http://coco.gforge.inria.fr/apidocs-cocoex/cocoex.html
  - Python `example_experiment.py`: http://coco.gforge.inria.fr/apidocs-example_experiment/example_experiment.html
  - Postprocessing code: http://coco.gforge.inria.fr/apidocs-cocopp/cocopp.html

* Somewhat outdated documents:
  - Full description of the platform: http://coco.lri.fr/COCOdoc/
  - Experimental setup before 2016: http://coco.lri.fr/downloads/download15.03/bbobdocexperiment.pdf
 - Old framework software documentation: http://coco.lri.fr/downloads/download15.03/bbobdocsoftware.pdf
 
 * Some examples of [results](https://github.com/numbbo/coco/wiki/COCO-Wiki-Home).

^1: see [#837](https://github.com/numbbo/coco/issues/837) and [#1266](https://github.com/numbbo/coco/issues/1266) for two pending errata

