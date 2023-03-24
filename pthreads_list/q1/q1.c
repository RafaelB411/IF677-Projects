#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Define a quantidade de arquivos, candidatos e threads, respectivamente
#define ARQ 5
#define CAND 3
#define THREADS 2

pthread_mutex_t mutex[CAND + 1] = { PTHREAD_MUTEX_INITIALIZER }; // Mutex do contador individual de cada candidato
pthread_mutex_t mutexTotal = PTHREAD_MUTEX_INITIALIZER;          // Mutex do contador do total de votos
int vet[CAND + 1] = {0}, total = 0;                              // Array com o contador de votos individuais e total de votos
int filesDone = 0;                                               // Contador de arquivos já lidos/sendo lidos

void *contador(void *arg){
    // A thread recebe o indice do arquivo que deve ler inicialmente
    int x = *(int*)arg, k;
    char filename[100];
    FILE *file;

    do {
        // Lê o arquivo linha por linha
        sprintf(filename, "./casos/%01d.txt", x+1);
        file = fopen(filename, "r");
        while(!feof(file)){
            fscanf(file, "%d", &k);

            // Soma a quantidade de votos do candidato na linha atual
            pthread_mutex_lock(&mutex[k]);
            vet[k]++;
            pthread_mutex_unlock(&mutex[k]);

            // Soma a quantidade total de votos
            pthread_mutex_lock(&mutexTotal);
            total++;
            pthread_mutex_unlock(&mutexTotal);
        }

        // Checa se ainda há um arquivo não lido, se houver, lê ele
        pthread_mutex_lock(&mutexTotal);
        if(filesDone < ARQ)
            x = filesDone++;
        else
            x = -1;
        pthread_mutex_unlock(&mutexTotal);
    } while(x != -1); // Repete enquanto houver um arquivo pra ler

    pthread_exit(NULL);
}

int main (int argc, char *argv[]){  
    // Threads e indice das threads 
    pthread_t threads[THREADS];
    int *taskids[THREADS];

    // Inicia o filesDone com a quantidade de threads (cada uma pega um inicialmente)
    filesDone += THREADS;
    for (int t = 0; t < THREADS; t++){
        taskids[t] = (int *)malloc(sizeof(int));
        *taskids[t] = t;
        pthread_create(&threads[t], NULL, contador, (void *)taskids[t]);
    }

    // Espera por todas as threads terminarem de contar
    for(int t = 0; t < THREADS; t++)
        pthread_join(threads[t], NULL);

    // Exibe o total de votos e a porcentagem de cada candidato/voto branco
    printf("%0.2f %% dos votos foram branco\n", (vet[0] / (float)total)*100);
    for(int j = 1; j <= CAND; j++){
        printf("candidato %d teve %0.2f %% de votos\n", j, (vet[j] / (float)total)*100);
    }
    printf("total de votos: %d\n", total);
}