# Call Graph


## Building

These instructions assume that your current directory starts out as the
"callgrapher" directory within the package.



```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

This produces a dynamic callgrapher tool called
tools/callgrapher/callgrapher and a library for printing out the
static call graph in *lib/callgraphs/callgraphs.so*.

## Testing

To generate call graph for the C file located in *test/simpletest.c* you
would do the following

```bash
$ cd test
$ make
```


## Example Output

```C
void foo(void);
void bar(void) {
        foo();
        bar();
}
void baz(void) {
        foo();
        bar();
}
int x (int i) {
        y(5);
}
int y (int i) {
        z(10);
}
int z (int i) {
        x(11);
}
int x_y_z (int i) {
        x(1);
}
int main(int argc, char **argv) {
        foo();
        bar();
        baz();
        void (*bam)(void) = 0;
        switch (argc%3) {
                case 0: bam = foo; break;
                case 1: bam = bar; break;
                case 2: bam = baz; break;
        }
        bam();
        return 0;
}
```

The code above generates graph that looks like this

![call graph for test/c/example.c](graph.png)


## Function Pointers

I used a dump way that compares function signatures. It checks the following:

- Number of Arguments.
- Class of return types (`LLVM::TypeID`)
- Class of arguments types (`LLVM::TypeID`)


