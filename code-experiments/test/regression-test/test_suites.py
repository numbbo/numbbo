#!/usr/bin/env python
"""test all known suites with data from 'data' folder, where
the optional input parameter is the number of data per
problem visible in the filename, by default 10.
"""
from __future__ import division, print_function
from ast import literal_eval  # safe evaluation/execution of Python code
import os, sys
import time
import numpy as np
import cocoex
try:
    from urllib.request import urlretrieve
except ImportError:
    from urllib import urlretrieve


def _is_equal(x, y):
    """return scalar or vector, where `x` and `y` can be a scalar
    or list/array_type
    """
    x, y = np.asarray(x), np.asarray(y)
    same_sign = x * y > 0
    ax, ay = np.abs(x), np.abs(y)
    lgx, lgy = np.log10(ax), np.log10(ay)
    return ((np.abs(x - y) < 1e-9) +  # "+" means in effect "or"
            same_sign * (np.abs(x - y) / (ax + ay) < 2e-9) +  # min(ax, ay) would be better?
            same_sign * (ax > 1e21) * (ay > 1e21)  # *  # because coco.h defines INFINITY possibly as 1e22
           ) # (np.abs(lgx - lgy) / (lgx + lgy) < 0.7) > 0)  # probably not very useful 

def is_equal(x, y):
    try:
        assert len(x) == len(y)
    except TypeError:
        """scalar case"""
        return _is_equal(x, y)
    else:
        return np.all(_is_equal(x, y))


def regression_test_a_suite(suite_name, filename):
    """filename contains previously generated test data to compare against.
    
    Details: on a Windows machine we see differences like
    f12 instance 58 in 2D (177, (1447.3149385050367, -830.3270488085931))
        1.7499057709942032e+141 vs 6.09043250958e+67 (original): log-err = 0.351...
    f17: 3.648247252180286e+57 vs 3.46559033612e+57: log-err = 0.0002
    f17 f17: [2.885437508322743e+22, 1322751113639934.8] vs [2.05085412e+22, 1.32275111e+15] or
    f14 f17: [31585031.800419718, 6.480639092419489e+28] vs [3.15850318e+07, 1.69518822e+28]: log-err = 0.01
    f16: -0.13227493309325666 vs -0.132274933067: rel-err = 9.9e-11
    problem f12 instance 60 in 10D: 1.2219569577863538e+76 vs 1.2963795943e+27: log-err = 0.47...
    bbob_f001_i02_d40__bbob_f017_i04_d40: [287089787.64410305, 3.1488536291123374e+39] vs [  2.87089788e+008   9.76803002e+118]: log-err = 0.50156...
    large-scale suite problem f17 instance 4 in 40D (5.449929602038278e+136, []) 6.96081091792e+29: log-err = 0.64...
    """
    verbose = 1
    xfc_dict = literal_eval(open(filename).read())
    if verbose:
        print("using file %s with %d test cases " % (filename, len(xfc_dict)), end="")
        sys.stdout.flush()
        t0 = time.clock()
    suite = cocoex.Suite(suite_name, "year: 0000", "") # choose "default" year for test
    failed_test_counter = 0
    for key in xfc_dict:
        f, x = suite[key[0]], key[1]
        try:
            assert is_equal(f(x), xfc_dict[key][0])
        except AssertionError:
            print(f.name, "id,x =", key, "stored f(x),con(x) =",
                  xfc_dict[key], "computed f(x) =", f(x))
            failed_test_counter += 1
        if f.number_of_constraints > 0:
            try:
                assert is_equal(f.constraint(x), xfc_dict[key][1])
            except AssertionError:
                print(f.name, "index,x =", key, "stored f,con =",
                      xfc_dict[key], "computed con =", f.constraint(x))
                failed_test_counter += 1
    if failed_test_counter > 0:
        raise AssertionError("{} assertions failed".format(failed_test_counter))
    if verbose:
        print("done in %.1fs" % (time.clock() - t0))


if __name__ == "__main__":
    try:
        ndata = int(sys.argv[1])
    except IndexError:
        ndata = 2
    except ValueError:
        print(__doc__)
    try:
        ndata
    except:
        pass
    else:
        for name in cocoex.known_suite_names:
            # Skip any toy suites
            if 'toy' in name:
                print("skipping suite {}".format(name))
                continue

            data_file_path = ("data/regression_test_%ddata_for_suite_" % ndata) + name + ".py"

            if not os.path.exists(data_file_path):
                remote_data_path = 'http://coco.gforge.inria.fr/regression-tests/'
                # download data from remote_data_path:
                if not os.path.exists(os.path.split(data_file_path)[0]):
                    try:
                        os.makedirs(os.path.split(data_file_path)[0])
                    except os.error:  # python 2&3 compatible
                        raise
                url = '/'.join((remote_data_path, data_file_path))
                print("  downloading %s to %s" % (url, data_file_path))
                urlretrieve(url, data_file_path)

            regression_test_a_suite(name, data_file_path)
    
    if 11 < 3: # test whether last dimensions of `bbob` and first of `bbob-largescale` match
        regression_test_a_suite("bbob-largescale",
                os.path.join("data",
                             "regression_test_%d-bbobdata_for_bbob-largescalesuite_" % ndata + ".py"))
