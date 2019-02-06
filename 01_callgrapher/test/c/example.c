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
