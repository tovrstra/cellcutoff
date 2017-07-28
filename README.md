CellList is a 3D domain decomposition library.

Dependencies
============

* C++11 compiler (tested: GNU and Intel)
* Python, >=2.7.x, <3.x
* Numpy, >1.9
* Cython, >= 0.24.1


Runtime environment configuration
=================================

The instructions below explain how to install everything in `${HOME}/.local`, such that
you do not need root permissions to install all the software. To make this work, some
environment variables must be set, e.g. in your `~/.bashrc`.

    export PATH=${HOME}/.local/bin:${PATH}
    export LD_LIBRARY_PATH=${HOME}/.local/lib:${HOME}/.local/lib64:${LD_LIBRARY_PATH}


Installation of `celllists`
===========================

Build (for installation in home directory):

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=${HOME}/.local
    make

Testing, in `build`:

    make check

Install, in `build`:

    make install


Installation of `python-celllists`
==================================

Build the wrapper:

    cd python-celllists
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=${HOME}/.local
    make

Testing, in `python-celllists/build`

    make test

Install, in `python-celllists/build`

    ./setup.py install --user
