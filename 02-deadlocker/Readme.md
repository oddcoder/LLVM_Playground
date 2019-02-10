# Deadlocker


Building
========

These instructions assume that your current directory starts out as the
"deadlocker" directory within the package.

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

This produces a dynamic deadlocker tool called
tools/deadlocker/deadlocker.

Running the double locking violation detector:
e.g.
clang -g -c -emit-llvm ../deadlocker/test/simpletest.c -o locky.bc
tools/deadlocker/deadlocker locky.bc
