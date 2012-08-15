TriangleRender
==============

simple GLUT application that renders triangle geometries for benchmarking

Building
========

CMake is required for building. From within the base TriangleRender directory:

1) mkdir build
2) cd build
3) ccmake ..

Configure and generate makefiles using the 'c' and 'g' keys.

4) make

Usage
=====

syntax: ./trianglerender [options] [geometry filenames...]
options:
 -l   use display lists (default false)

Example: ./trianglerender -l *.tri
