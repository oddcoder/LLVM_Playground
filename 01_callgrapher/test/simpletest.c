
void foo();

void bar() {
  foo();
  bar();
}

void baz() {
  foo();
  bar();
}

int main(int argc, char **argv) {
  foo();
  bar();
  baz();
  void (*bam)() = 0;
  switch (argc%3) {
   case 0: bam = foo; break;
   case 1: bam = bar; break;
   case 2: bam = baz; break;
  }
  bam();
  return 0;
}

