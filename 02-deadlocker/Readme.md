# Deadlocker

Deadlocker is tool implemented to capture undefined states of pthread mutexes,
it is originally written following [Project 2: Interprocedural Dataflow](
http://www.cs.sfu.ca/~wsumner/teaching/886/15/project2.html).

Ideally, A mutex should first be initialized by `pthread_mutex_init` function,
after that it can be locked and unlocked as much as needed.

Deadlocker captures the following mutex misuses:-

- Locking or unlocking uninitialized mutex.
- Mutex double locking.
- Mutex double unlocking.

Deadlocker implements interprocedural context-sensitive, flow-sensitive, mutex
abstract state data flow analysis. It uses built in alias analysis to keep track
of pointers referencing the same mutex. However, it is advised to always compile
the target module using `-O2 -mllvm -disable-llvm-optzns` and make a `-mem2reg`
transformation before invoking deadlocker.

## Building

These instructions assume that your current directory starts out as the
"deadlocker" directory within the package.

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Usage

```
$ clang -g -emit-llvm -O2 -mllvm -disable-llvm-optzns -S [target_file.c] -o - | opt -mem2reg -S -o [target_file.ll]
$ deadlocker [target_file.ll]
```

## Example

[![asciicast](https://asciinema.org/a/6RWiDTSIIbZgVIX9AD9bHqOJ2.svg)](https://asciinema.org/a/6RWiDTSIIbZgVIX9AD9bHqOJ2)
