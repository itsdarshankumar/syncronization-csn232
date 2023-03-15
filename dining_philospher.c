#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>
#include<pthread.h>

sem_t ch[5];
void *Philosphers(void *);

int main(){
	int num[5];
	pthread_t Threads[5];
	for(int i=0;i<5;i++)
		sem_init(&ch[i],0,1);
	for(int i=0;i<5;i++){
		num[i]=i;
		pthread_create(&Threads[i],NULL,Philosphers,(void*)&num[i]);

	}
	for(int i=0;i<5;i++)
		pthread_join(Threads[i],NULL);
}


void *Philosphers(void *num){
	int ind=*(int *)num;
	//
	printf("Philospher %d is hungry",ind);
	printf("\n");
	printf("Philospher %d tries to pick chopstick to his left",ind);
	printf("\n");
	sem_wait(&ch[ind]);
	printf("Philospher %d picks chopstick to his left",ind);
	printf("\n");
	printf("Philospher %d tries to pick chopstick to his right",ind);
	printf("\n");
	//
	sem_wait(&ch[(ind+1)%5]);
	printf("Philospher %d picks chopstick to his right",ind);
	printf("Philospher %d starts eating",ind);
	sleep(5);
	printf("Philospher %d is done with eating",ind);
	//
	sem_post(&ch[(ind+1)%5]);
	printf("Philospher %d puts down chopstick to his right, down",ind);
	//
	sem_post(&ch[ind]);
	printf("Philospher %d puts down chopstick to his left, down",ind);
}