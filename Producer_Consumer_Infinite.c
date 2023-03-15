#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sem.h"
#include <unistd.h>

#define production_time 1
#define consumption_time 1

struct node {
    int data;
    struct node* next;
};


struct node *in=NULL, *out;
semaphore mutex;
semaphore size;



void* producer(void *i){
    while (true) {
        int item = rand()%31;
        sleep(production_time);
        sem_wait(&mutex);
        struct node *tmp =(malloc(sizeof(struct node)));
        in->data=item;
        in->next=tmp;
        in = tmp;
        printf("producer-%d produces : ",i);
        printf("%d \n", item);
        sem_post(&size);
        sem_post(&mutex);
        sleep(production_time);
    }
}


void* consumer(void *i){
    while (true) {
        int item;
        sem_wait(&size);
        sem_wait(&mutex);
        item = out->data;
        out=out->next;
        printf("consumer-%d consumes : ",i);
        printf("%d \n", item);
        sem_post(&mutex);
        sleep(consumption_time);
    }
}


int main() {
    srand(time(NULL));
    semaphore_initialization(&size, 0);
    semaphore_initialization(&mutex, 1);
    in = (malloc(sizeof(struct node)));
    in->data=NULL;
    out = in;
    int producers, consumers;
    scanf("%d %d", &producers, &consumers);
    pthread_t threads[producers+consumers];
    for (int i = 1; i <= producers; ++i) {
        pthread_create(&threads[i], NULL, producer, (void*)i);
    }
    for (int i = 1; i <= consumers; ++i) {
        pthread_create(&threads[i], NULL, consumer, (void*)i);
    }
    for (int i = 1; i <= producers+consumers; ++i) {
        pthread_join(threads[i], NULL);
    }


    return 0;

}
