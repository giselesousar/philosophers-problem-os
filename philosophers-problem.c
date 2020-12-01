/*
Author: Gisele Ribeiro

gcc -pthread -o philosophers-problem philosophers-problem.c
./philosophers-problem

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#define N 5 //quantidade de threads

#define LEFT (i+N-1)%N //vizinho esquerdo
#define RIGHT (i+1)%N //vizinho direito

#define THINKING 0
#define HUNGRY 1
#define EATING 2

pthread_mutex_t mutex; //regula o acesso a regiao critica
pthread_cond_t p[N]; //variaveis de condicao dos filosofos

int state[N]; //estado atual de cada filosofo {0, 1 , 2} 

void print_states(){
    int i;
    int eating = 0;

    printf(" \n Vetor de estados atual: [");
    for(i = 0; i < N; i++){
        printf(" %d ", state[i]);
        if(state[i] == EATING){
            eating++;
        }
    }
    if(eating == 1){
        printf(" ] --> %d filosofo esta comendo \n", eating);
    }else{
        printf(" ] --> %d filosofos estao comendo \n", eating);
    }
    
}

/*
Parametros: indice referente a uma thread 
a funcao verifica se o estado da thread eh 1 e se seus vizinhos nao estao com estado 2
retorna 1 caso a condicao seja verdadeira, mudando seu estado para 2 e 
sinaliza a thread em questao para desperta-la
retorna 0 quando a condicao nao eh satisfeita
*/
int test(int i){
   
    if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING){

       state[i] = EATING;


       pthread_cond_signal(&p[i]);

       print_states();

       return 1;
    }
    return 0;
}
/*
parametros: indice referente a uma thread 
a funcao muda o estado da thread para 1 e faz uma chamada a funcao test 
se test retorna 0, a thread nao mudou seu estado para 2 e 
entao eh bloqueada
*/
void take_forks(int *i){
    pthread_mutex_lock(&mutex);
    state[*i] = HUNGRY;

    print_states();
    
    int t = test(*i);

    if(t == 0){
        pthread_cond_wait(&p[*i], &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

/*
Parametros: indice referente a uma thread
a funcao muda o estado da thread para 0 
e chama a funcao test passando como parametro as threads vizinhas
*/
void put_forks(int *i){
    pthread_mutex_lock(&mutex);
    state[*i] = THINKING;

    print_states();

    int t = test((*i+N-1)%N);
    t = test((*i+1)%N);
    pthread_mutex_unlock(&mutex);
}

//funcao invocada por cada thread para iniciar sua execucao
void* philosopher(void *context){

    int *i = context;

    while(1){
        sleep(1+ rand()%5); //thinking
        take_forks(i);
        sleep(1+ rand()%8); //eating
        put_forks(i);
    }

    return NULL;
}

int main(){

    pthread_t *philosophers = malloc(sizeof(*philosophers)*N);

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



