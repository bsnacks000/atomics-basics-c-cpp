#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define SPIN

typedef struct {
  u_int64_t value;
#ifdef SPIN
  pthread_spinlock_t lock;
#else
  pthread_mutex_t lock;
#endif

} data;

void* worker(void* arg) {
  data* d = (data*)arg;

  for (int i = 0; i < 100000; i++) {
#ifdef SPIN
    pthread_spin_lock(&d->lock);
#else
    pthread_mutex_lock(&(d->lock));
#endif
    d->value++;
#ifdef SPIN
    pthread_spin_unlock(&d->lock);
#else
    pthread_mutex_unlock(&(d->lock));
#endif
  }

  return NULL;
}

int main(void) {
  data d;

  d.value = 0;
#ifdef SPIN
  pthread_spin_init(&d.lock, 0);
#else
  pthread_mutex_init(&d.lock, NULL);
#endif

  pthread_t a;
  pthread_t b;

  int err;
  if ((err = pthread_create(&a, NULL, worker, (void*)&d)) != 0) {
    fprintf(stderr, "err: create: %d", err);
    exit(1);
  }

  if ((err = pthread_create(&b, NULL, worker, (void*)&d)) != 0) {
    fprintf(stderr, "err: create: %d", err);
    exit(1);
  }

  if ((err = pthread_join(a, NULL)) != 0) {
    fprintf(stderr, "err: join a: %d", err);
    exit(1);
  }

  if ((err = pthread_join(b, NULL)) != 0) {
    fprintf(stderr, "err: join b: %d", err);
    exit(1);
  }

#ifdef SPIN
  if ((err = pthread_spin_destroy(&d.lock)) != 0) {
    fprintf(stderr, "err: destory spin: %d", err);
    exit(1);
  }
#else

  if ((err = pthread_mutex_destroy(&d.lock)) != 0) {
    fprintf(stderr, "err: destory spin: %d", err);
    exit(1);
  }
#endif

  printf("value: %lu\n", d.value);

  return 0;
}