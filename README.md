# OpenGLVM
Read and generate a Virtual Machine from the Khronos OpenGL specifications and run only OpenGL code through it.
The project goal is to establish a reusable bytecode language which can be used by multiple VM implementations.
This byte code only support OpenGL functions, no math, logic programming, system functions or so ever.

## libSpec
A library to read the OpenGL specification files and build a tree representation.

## CodeGenerator

A console tool to generate a simple Virtual Machine which understand OpenGLVM byte code.

## Technology

A multi threaded test application which generate byte code on multiple threads and run it on the render thread.
This allow to split the render work across all CPU cores.
