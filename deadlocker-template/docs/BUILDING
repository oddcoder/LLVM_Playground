
These instructions assume that your current directory starts out as the
"deadlocker" directory within the package.


Building with CMake
==============================================

1. Create a new directory for building.
  e.g. mkdir ../deadlockerbuild

2. Change into the new directory.
  e.g. cd ../deadlockerbuild

3. Run CMake with the path to the LLVM source. For LLVM 3.5
and later, LLVM can be built with configure even if the deadlocker is built
with CMake.
  e.g. cmake -DLLVM_DIR=</path/to/LLVM/build>/share/llvm/cmake/ ../deadlocker

4. Run make inside the build directory.
  e.g. make

This produces a dynamic deadlocker tool called
tools/deadlocker/deadlocker.

Running the double locking violation detector:
e.g.
clang -g -c -emit-llvm ../deadlocker/test/simpletest.c -o locky.bc
tools/deadlocker/deadlocker locky.bc


Building with Autoconf / configure
==============================================

1. Download and build LLVM as per the standard instructions.
  (http://llvm.org/docs/GettingStarted.html#getting-started-quickly-a-summary)
  
2. Reconfigure the deadlocker project to let it know where you built LLVM.
  e.g.
  cd autoconf
  ./AutoRegen.sh
  <Enter the paths to the LLVM source and build directories when asked>
  cd ..

  NOTE:
  For Ubuntu with llvm 3.5 or later you can use e.g.:
  /usr/lib/llvm-3.5/build/

3. Create a new directory for building.
  e.g. mkdir ../deadlockerbuild

4. Change into the new directory.
  e.g. cd ../deadlockerbuild

5. Run configure from the new build directory. NOTE: You must configure the
  project with the same debugging and assertion options as you configured LLVM.
  That is, if you enabled LLVM debugging with
  --disable-optimized --enable-debug-runtime --enable-assertions
  then you should do the same for the project
  e.g. ../deadlocker/configure --with-llvmsrc=<path to llvm src dir> --with-llvmobj=<path to llvm build dir> --disable-optimized --enable-debug-runtime --enable-assertions

  Or for Ubuntu 14.10 in particular:
  ../deadlocker/configure --with-llvmsrc=/usr/lib/llvm-3.5/build/ --with-llvmobj=/usr/lib/llvm-3.5/build/ --disable-optimized --enable-debugging

6. Run make inside the build directory.
  e.g. make

This produces a deadlocker tool in a folder that depends on your
configuration options. If you have assertions and debugging enabled, it will be
Debug+Asserts/bin/deadlocker
If you have assertions enabled and debugging disabled, it will be
Release+Asserts/bin/deadlocker

Running the double locking violation detector:
e.g.
clang -g -c -emit-llvm ../deadlocker/test/simpletest.c -o locky.bc
Debug+Asserts/bin/deadlocker locky.bc

