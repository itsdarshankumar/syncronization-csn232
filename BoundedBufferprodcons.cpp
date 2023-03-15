#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include<stack>
#include "sem.h"
#define b_size 5
#define production_time 1
#define consumption_time 1

std::stack<int>buffer;
int item =0;
semaphore full, empty, mutex;


void* producer(void *i){
    while (1) {
        //int item;
        // Produces
        sleep(production_time);
        sem_wait(&empty);
        sem_wait(&mutex);
        item++;
        printf("producer-%d ",i);

        buffer.push(item);
        printf("%d ",buffer.size());
        printf( "\n");
        sem_post(&mutex);
        sem_post(&full);
    }
}


void* consumer(void *i){
    while (1) {
        int consumed;
        sem_wait(&full);
        sem_wait(&mutex);
        consumed = buffer.top();
        buffer.pop();
        printf("consumer-%d ",i);
        printf("%d ",buffer.size());
        printf( "\n");
        sem_post(&mutex);
        sem_post(&empty);
        //Consumes
        sleep(consumption_time);
    }
}


int main() {
    srand(time(NULL));
    semaphore_initialization(&full, 0);
    semaphore_initialization(&empty, 5);
    semaphore_initialization(&mutex, 1);
    int producers, consumers;
    scanf("%d %d", &producers, &consumers);
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
