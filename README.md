TriangleRender
==============

simple GLUT application that renders triangle geometries for benchmarking

Building
========

CMake is required for building. From within the base TriangleRender directory:

    mkdir build
    cd build
    ccmake ..

Configure and generate makefiles in CMake using the 'c' and 'g' keys.

    make

Usage
=====

    syntax: ./trianglerender [options] [geometry filenames...]
    options:
     -l   use display lists (default false)
     -z   zoom factor (default 1, must be >= 1)

Example:

    ./trianglerender -l *.tri
