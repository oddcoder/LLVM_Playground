
#include<pthread.h>
#include<stdio.h>

pthread_mutex_t a;
pthread_mutex_t b;

void
acquireTwo(pthread_mutex_t *m1, pthread_mutex_t *m2) {
  pthread_mutex_lock(m2);
  puts("Twice Locked\n");
  pthread_mutex_unlock(m1);
}

int
main() {
  pthread_mutex_init(&a, NULL);
  pthread_mutex_init(&b, NULL);
  puts("Before\n");
  pthread_mutex_lock(&a);
  acquireTwo(&a, &b);
  pthread_mutex_unlock(&b);
  puts("Twice Unlocked\n");
  pthread_mutex_lock(&a);
  puts("After More\n");
  return 0;
}

