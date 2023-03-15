#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sem.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_CUSTOMERS 2
#define NUM_CUSTOMERS 4

int cust_sitting = 0;
int cust_waiting = 0;
bool has_waiting = false;

semaphore* block;
semaphore* mutex;

void *customer_sushi(void *num) {

    int ind = *(int *)num;
    sem_wait(&mutex);
    if (has_waiting) {
        cust_waiting++;
        printf("Customer %d waiting\n", ind);
        sem_post(&mutex);
        sem_wait(&block);
    }
    else {
        cust_sitting++;
        printf("Customer %d seated\n", ind);
        has_waiting = (cust_sitting == 5);
        sem_post(&mutex);
    }

    printf("Customer %d having sushi\n", ind);

    sem_wait(&mutex);
    printf("Customer %d finished eating\n", ind);
    cust_sitting--;
    if (cust_sitting == 0) {
        if (cust_waiting < 5) {
            for (int i = 0; i < cust_waiting; i++) {
                sem_post(&block);
            }
            cust_sitting += cust_waiting;
            cust_waiting = 0;
            has_waiting = (cust_sitting==5);
        }
        else {
            cust_waiting-=5;
            cust_sitting+=5;
            has_waiting = true;
            for (int i = 0; i < 5; i++) {
                sem_post(&block);
            }
        }
    }
    sem_post(&mutex);

}

int main() {

    semaphore_initialization(&block, 0);
    semaphore_initialization(&mutex, 1);

    pthread_t customers_threads[NUM_CUSTOMERS];
    int id[NUM_CUSTOMERS];

    for (int i = 0; i < NUM_CUSTOMERS; i++) {

        id[i] = i;
        pthread_create(&customers_threads[i],NULL,customer_sushi,&id[i]);

    }

    for (int i = 0; i < NUM_CUSTOMERS; i++) {

        pthread_join(customers_threads[i],NULL);

    }

}