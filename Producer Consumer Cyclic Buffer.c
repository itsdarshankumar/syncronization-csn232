#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sem.h"
#include <unistd.h>

#define b_size 10
#define production_time 1
#define consumption_time 1
#define producers 5
#define consumers 5

int buffer[b_size];
int in=0, out=0, size=0;
semaphore full, empty, mutex;




void* producer(void *args){
    while (1) {
        int item;
        // Produces
        item = rand()%100;
        printf("Producer-%d produced: %d \n", args, item);
        sleep(production_time);
        sem_wait(&empty);
        sem_wait(&mutex);
        buffer[in] = item;
        in = (in + 1) % b_size;
        size++;
        sem_post(&mutex);
        sem_post(&full);
    }
}


void* consumer(void *args){
    while (1) {
        int item;
        sem_wait(&full);
        sem_wait(&mutex);
        item = buffer[out];
        out = (out + 1) % b_size;
        size--;
        sem_post(&mutex);
        sem_post(&empty);
        //Consumes
        printf("Consumer-%d consumed: %d \n", args, item);
        sleep(consumption_time);
    }
}


int main() {
    srand(time(NULL));
    semaphore_initialization(&full, 0);
    semaphore_initialization(&empty, b_size);
    semaphore_initialization(&mutex, 1);
    pthread_t threads[producers+consumers];
    for (int i = 0; i < producers; ++i) {
        pthread_create(&threads[i], NULL, producer, (void*)i);
    }
    for (int i = 0; i < consumers; ++i) {
        pthread_create(&threads[i], NULL, consumer, (void*)i);
    }
    for (int i = 0; i < producers+consumers; ++i) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
