#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#define N 5 //quantidade de threads

#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N

#define THINKING 0
#define HUNGRY 1
#define EATING 2

pthread_mutex_t mutex;
pthread_cond_t p[N];

int state[N];


int test(int i){
   
    if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING){
       state[i] = EATING;
       printf("Thread %d is eating! \n", i);
       pthread_cond_signal(&p[i]);
       return 1;
    }
    return 0;
}

void take_forks(int *i){
    pthread_mutex_lock(&mutex);
    state[*i] = HUNGRY;
    
    int t = test(*i);

    if(t == 0){
        pthread_mutex_unlock(&mutex);
        pthread_cond_wait(&p[*i], &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

void put_forks(int *i){
    pthread_mutex_lock(&mutex);
    state[*i] = THINKING;
    int t = test((*i+N-1)%N);

    t = test((*i+1)%N);

    pthread_mutex_unlock(&mutex);
}


void* philosopher(void *context){

    int *i = context;

    while(1){
        printf("Thread %d is thinking! \n", *i);
        sleep(1+ rand()%5); //think
        printf("Thread %d is hungry! \n", *i);
        take_forks(i);
        
        sleep(1+ rand()%8); //eat
        printf("Thread %d put forks! \n", *i);
        put_forks(i);
    }

    return NULL;
}

int main(){

    pthread_t *philosophers = malloc(sizeof(*philosophers)*5);

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < N; i++){

        int* id = malloc(sizeof(int));
        *id = i;

        pthread_cond_init(&p[*id], NULL);

        pthread_create(&philosophers[i], NULL, philosopher, id);
        
    }
    
    pthread_exit(NULL); 

    return 0;
}