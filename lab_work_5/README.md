 # Lab work 5

## Software required to build the program
- [CMake](https://cmake.org/)'s meta build system, which creates build files for a particular environment
- The build system that will build the program, such as [GNU Make](https://www.gnu.org/software/make/)
- Compiler, such as [GCC](https://gcc.gnu.org/)

## Building

1. Go to "lab_work_5" folder
2. Run the following command:
```sh
cmake -S src -B build && cmake --build build
```
You can select a generator thanks to the "-G" flag of the "cmake" program. Then the command to build the program might look like this:
```sh
cmake -S src -B build -G "CodeBlocks - Unix Makefiles" && cmake --build build
```
If the build command completes successfully, a program file named "program" will appear in the "build" directory.

## Launching

1. Go to "lab_work_5/build" folder
2. Run the following command:
```sh
./program
```
