#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "semaphore.h"
#include <unistd.h>

int PotSize= 10;
int Savage_Count= 5;
int Hunger =7;
semaphore pot_mutex, served, empty_pot;
int servings = 0;


void *savage(void *arg) {
    int k = (__intptr_t)arg;
    for(int i=0;i<Hunger;i++) {
        sem_wait(&pot_mutex); // Wait for pot_mutex
        if (servings == 0) { // If pot is empty
            sem_post(&empty_pot); // Signal empty_pot
            sem_wait(&served); // Wait for servings
        }
        servings--; // Decrement servings
        printf("Savage %d ate, servings left: %d\n",k,servings);
        sem_post(&pot_mutex); // Signal pot_mutex
        sleep(rand() % 3); // Digest for a random amount of time
        
    }
    return NULL;
}

void *cook(void *arg) {
    int WakeUps=(((Savage_Count*Hunger)+PotSize-1)/PotSize);
    for(int i=0;i<WakeUps;i++){
        sem_wait(&empty_pot); // Wait for empty_pot
        printf("Cook is refilling the pot\n");
        servings = PotSize; // Refill pot
        sem_post(&served); // Signal served
    }
    return NULL;
}

int main() {
    pthread_t savage_threads[Savage_Count], cook_thread;

    semaphore_initialization(&pot_mutex, 1); // Initialize pot_mutex semaphore to 1
    semaphore_initialization(&served, 0); // Initialize served semaphore to 1
    semaphore_initialization(&empty_pot, 0); // Initialize empty_pot semaphore to 0

    // Create savage threads
    for (int i = 0; i < Savage_Count; i++) {
        pthread_create(&savage_threads[i], NULL, savage, (void *)(__intptr_t)i);
    }

    // Create cook thread
    pthread_create(&cook_thread, NULL, cook, NULL);

    // Join threadsS
    for (int i = 0; i < Savage_Count; i++) {
        pthread_join(savage_threads[i], NULL);
    }
    pthread_join(cook_thread, NULL);

    return 0;
}
