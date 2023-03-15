#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sem.h"
#include <unistd.h>

semaphore tobacco;
semaphore paper;
semaphore matches;

void *agent(void *arg) {
    while(1) {
        int choice = rand() % 3;

        if(choice == 0) {
            sem_post(&tobacco);
            sem_post(&paper);
            printf("Agent puts tobacco and paper on the table\n");
        }
        else if(choice == 1) {
            sem_post(&tobacco);
            sem_post(&matches);
            printf("Agent puts tobacco and matches on the table\n");
        }
        else {
            sem_post(&paper);
            sem_post(&matches);
            printf("Agent puts paper and matches on the table\n");
        }

        sleep(1);
    }
}

void *smoker_tobacco(void *arg) {
    while(1) {
        sem_wait(&paper);
        sem_wait(&matches);

        printf("Smoker with tobacco takes paper and matches from table and starts smoking\n");

        sem_post(&tobacco);

        sleep(1);
    }
}

void *smoker_paper(void *arg) {

    while(1) {
        sem_wait(&tobacco);
        sem_wait(&matches);

        printf("Smoker with paper takes tobacco and matches from table and starts smoking\n");

        sem_post(&paper);

        sleep(1);
    }
}

void *smoker_matches(void *arg) {
    while(1) {
        sem_wait(&tobacco);
        sem_wait(&paper);

        printf("Smoker with matches takes tobacco and paper from table and starts smoking\n");

        sem_post(&matches);

        sleep(1);
    }
}

int main() {
    semaphore_initialization(&tobacco, 0);
    semaphore_initialization(&paper, 0);
    semaphore_initialization(&matches, 0);

    pthread_t agent_thread;
    pthread_t smoker_tobacco_thread;
    pthread_t smoker_paper_thread;
    pthread_t smoker_matches_thread;

    pthread_create(&agent_thread, NULL, agent, NULL);
    pthread_create(&smoker_tobacco_thread, NULL, smoker_tobacco, NULL);
    pthread_create(&smoker_paper_thread, NULL, smoker_paper, NULL);
    pthread_create(&smoker_matches_thread, NULL, smoker_matches, NULL);

    pthread_join(agent_thread, NULL);
    pthread_join(smoker_tobacco_thread, NULL);
    pthread_join(smoker_paper_thread, NULL);
    pthread_join(smoker_matches_thread, NULL);

    return 0;
}
