#include <iostream>
#include <pthread.h>
#include "sem.h"
void *writer(void *);
void *reader(void *);
semaphore rd_mutex,wrt_mutex,in_mutex;
pthread_t w[5],r[10];//total number of writers and readers
int r_count=0;//will give the number of reader entered
int shared_variable=0;
void *writer(void *i){
    sem_wait(&in_mutex);
    printf("writer-%d is in the ENTRY_SECTION\n",i);
    sem_wait(&wrt_mutex);
    shared_variable+=10;//critical section
    printf("writer-%d is writing value => %d \n",i,shared_variable);
    sem_post(&wrt_mutex);
    printf("writer-%d is EXITING\n",i);
    sem_post(&in_mutex);
}
void *reader(void *i){
    sem_wait(&in_mutex);
    printf("reader-%d is in the ENTRY_SECTION\n",i);
    sem_wait(&rd_mutex);
    r_count++;
    if(r_count==1) sem_wait(&wrt_mutex);
    sem_post(&rd_mutex);
    sem_post(&in_mutex);
    printf("reader-%d is reading value => %d \n",i,shared_variable);//critical section
    sem_wait(&rd_mutex);
    r_count--;
    if(r_count==0) sem_post(&wrt_mutex);
    printf("reader-%d is EXITING\n",i);
    sem_post(&rd_mutex);

}
int main(){
    //initialization
    semaphore_initialization(&in_mutex,1);
    semaphore_initialization(&rd_mutex,1);
    semaphore_initialization(&wrt_mutex,1);
    //reader threads
    for(int i=0;i<10;i++){
        pthread_create(&r[i],NULL,reader,(void *)i);
    }
    //writer threads
    for(int i=0;i<5;i++){
        pthread_create(&w[i],NULL,writer,(void *)i);
    }
    for(int i=0;i<10;i++){
        pthread_join(r[i],NULL);
    }
    for(int i=0;i<5;i++){
        pthread_join(w[i],NULL);
    }
}
