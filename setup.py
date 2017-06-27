#!/usr/bin/env python

from setuptools import setup
from distutils.sysconfig import get_python_lib
import glob
import os
import sys

if os.path.exists('README.md'):
    print("""The setup.py script should be executed from the build directory.

Please see the file 'README.md' for further instructions.""")
    sys.exit(1)


setup(
    name = "celllistsexample",
    package_dir = {'': 'pycelllists'},
    data_files = [(get_python_lib(), glob.glob('pycelllists/*.so')),
                    ],
    author = 'Toon Verstraelen & Matt Chan',
    description = 'Use the CMake build system to make Cython modules.',
    license = 'GPLv3',
    keywords = 'cmake cython build',
    url = 'https://github.com/QuantumElephant/celllists',
    zip_safe = False,
    )
