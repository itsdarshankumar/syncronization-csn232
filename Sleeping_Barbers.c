#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sem.h"
#include <stdint.h> 
#include <inttypes.h> 
#include <time.h>
#include <unistd.h>

#define QUEUE_SIZE 100

int TOTAL_BARBERS	= 3;
int AV_CHAIRS = 10;
int COUNT_CUSTOMERS = 25;
int CUSTOMER_MAX_INTERVAL = 3;
int HAIRCUT_TIME = 8; 

typedef struct {
    int customer_id;
} customer_t;

typedef struct {
    customer_t queue[QUEUE_SIZE];
    int front;
    int rear;
    int size;
} queue_t;

queue_t queue;

semaphore s, barber, queue_mutex, customer_ready;

void enqueue(customer_t customer) {
    sem_wait(&queue_mutex);
    if (queue.size == QUEUE_SIZE) {
        printf("Error: queue is full\n");
        exit(1);
    }
    queue.queue[queue.rear] = customer;
    queue.rear = (queue.rear + 1) % QUEUE_SIZE;
    queue.size++;
    sem_post(&queue_mutex);
}

customer_t dequeue() {
    sem_wait(&queue_mutex);
    if (queue.size == 0) {
        printf("Error: queue is empty\n");
        exit(1);
    }
    customer_t customer = queue.queue[queue.front];
    queue.front = (queue.front + 1) % QUEUE_SIZE;
    queue.size--;
    sem_post(&queue_mutex);
    return customer;
}

void* haircut_job(void* args)
{

	int i = (intptr_t)args;

	while(AV_CHAIRS == 0){
		printf("Customer %d is waiting\n", i);
		sleep(5);
	}  
	if(AV_CHAIRS >= 1 && AV_CHAIRS <= 10) {		

		sem_wait(&s);
		AV_CHAIRS--;
		printf("Customer %d entered the waiting room. %d chairs are available\n", i, AV_CHAIRS);
		sem_post(&s);

		customer_t customer;
		customer.customer_id = i;
		enqueue(customer);
		sem_post(&customer_ready);

		sem_wait(&barber);
		AV_CHAIRS++;
		printf("Customer %d went with barber. %d chairs are available\n", i, AV_CHAIRS);

        
		sleep(HAIRCUT_TIME);
		printf("Customer %d received haircut.\n", i);
		sem_post(&barber);
	}
	pthread_exit(NULL);
	return NULL;
}

void* barber_job(void* args)
{
    while (1) {
        sem_wait(&customer_ready);
        sem_wait(&barber);
        customer_t customer = dequeue();
        AV_CHAIRS++;
        printf("Barber is cutting hair for customer %d. %d chairs are available\n", customer.customer_id, AV_CHAIRS);
        sem_post(&s);
        sleep(HAIRCUT_TIME);
        printf("Barber finished cutting hair for customer %d\n", customer.customer_id);
        sem_post(&barber);
    }
}

int main(int argc, char **argv)
{
	pthread_t threads[COUNT_CUSTOMERS];

	semaphore_initialization(&s, 1);
	semaphore_initialization(&barber, TOTAL_BARBERS);
    semaphore_initialization(&queue_mutex, 1);
	semaphore_initialization(&customer_ready, 0);
	
	if (argc > 1){
		TOTAL_BARBERS = atoi(argv[1]);
		if (TOTAL_BARBERS < 1){
			printf("There must be at least one barber\n");
			return(1);
		}
	}
	if (argc > 2){
		AV_CHAIRS = atoi(argv[2]);
		if (AV_CHAIRS < 1){
			printf("There must be at least one chair\n");
			return(1);
		}
	}
	if (argc > 3){
		COUNT_CUSTOMERS = atoi(argv[3]);
		if (COUNT_CUSTOMERS < 1){
			printf("There must be at least one customer\n");
			return(1);
		}
	}
	if (argc > 4){
		CUSTOMER_MAX_INTERVAL = atoi(argv[4]);
		if (CUSTOMER_MAX_INTERVAL < 0){
			printf("The customers cannot come in negative time\n");
			return(1);
		}
	}
	if (argc > 5){
		HAIRCUT_TIME = atoi(argv[5]);
		if (HAIRCUT_TIME < 1){
			printf("Haircuts must take at least one second\n");
			return(1);
		}
	}

	printf("This is the barbershop problem!\nFor this simulation, we have:\n");
	printf("%i Barbers\n", TOTAL_BARBERS);
	printf("%i Chairs\n", AV_CHAIRS);
	printf("%i customers will come in with delay from 0 to %i between them.\n", COUNT_CUSTOMERS, CUSTOMER_MAX_INTERVAL);
	printf("A haircut takes %i seconds.\n\n", HAIRCUT_TIME);
	printf("Beginning Simulation.\n\n");

	for(int it=0; it<COUNT_CUSTOMERS; ++it){
		pthread_create(&threads[it], NULL, haircut_job, (void*)(intptr_t)(it+1));
		sleep(1);
	}

	for(int it=0; it<COUNT_CUSTOMERS; ++it){
		pthread_join(threads[it], NULL);		
	}
	semaphore_destroy(&s);
	semaphore_destroy(&barber);
    semaphore_destroy(&queue_mutex);
    semaphore_destroy(&customer_ready);
	
	return(0);
}