
#include<pthread.h>
#include<stdio.h>

int
main() {
  pthread_mutex_t m;
  pthread_mutex_init(&m, NULL);
  puts("Before\n");
  pthread_mutex_lock(&m);
  puts("Locked\n");
  pthread_mutex_unlock(&m);
  puts("After\n");
  return 0;
}

