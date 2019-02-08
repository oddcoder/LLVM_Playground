
#include<pthread.h>
#include<stdio.h>

pthread_mutex_t a;
pthread_mutex_t b;

void
acquireTwo(pthread_mutex_t *m1, pthread_mutex_t *m2) {
  puts("Before\n");
  pthread_mutex_lock(m1);
  pthread_mutex_lock(m2);
  puts("Locked\n");
  pthread_mutex_unlock(m1);
  pthread_mutex_unlock(m2);
  puts("After\n");
}

void
pass1(pthread_mutex_t *m1, pthread_mutex_t *m2) {
  acquireTwo(m1,m2);
}

void
pass2(pthread_mutex_t *m1, pthread_mutex_t *m2) {
  pass1(m1,m2);
}

int
main() {
  pthread_mutex_init(&a, NULL);
  pthread_mutex_init(&b, NULL);
  pass2(&a, &b);
  pass2(&b, &a);
  return 0;
}

