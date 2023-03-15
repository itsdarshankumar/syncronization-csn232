#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>


typedef struct {
    atomic_int value;
} semaphore;

void semaphore_initialization(semaphore *semaphore_t, int value) {
    atomic_init(&semaphore_t->value, value);
}

void semaphore_destroy(semaphore *semaphore_t) {
    free(semaphore_t);
}

void sem_wait(semaphore *semaphore_t) {
    int value;

    do {
        value = atomic_load(&semaphore_t->value);
    } while (value <= 0 || !atomic_compare_exchange_weak(&semaphore_t->value, &value, value - 1));
}

void sem_post(semaphore *semaphore_t) {
    atomic_fetch_add(&semaphore_t->value, 1);
}