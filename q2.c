#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h> // Lib do sleep
 
#define TAM 2       // Limite máximo de trens por intersecção
#define INTER 5     // Quantidade de Intersecções
#define THREADS 10  // Quantidade de Threads
 
pthread_cond_t SIT_VAR[INTER] = { PTHREAD_COND_INITIALIZER }; // Variável de condição na qual os trens esperam
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;            // Mutes pra alterar o array buff
int buff[INTER] = { 0 };                                      // Quantidade de trens em cada intersecção
 
void interExit(int i, int tInd){
   // Sai da sua intersecção atual
   pthread_mutex_lock(&mutex);
   buff[i]--;
   printf("O trem %02d saiu da intersecçao %d\n", tInd + 1, i + 1);
   pthread_cond_signal(&SIT_VAR[i]);
   pthread_mutex_unlock(&mutex);
}
 
void interEntrance(int i, int tInd){
   // Tenta se mover para a próxima intersecção espera caso esteja cheia
   pthread_mutex_lock(&mutex);
   while(buff[i] == 2){
       pthread_cond_wait(&SIT_VAR[i], &mutex);
   }
   buff[i]++;
   printf("O trem %02d esta passando pela intersecçao %d\n", tInd + 1, i + 1);
   pthread_mutex_unlock(&mutex);
}
 
void *trem(void *threadid){
   // Armazena a intersecção pela qual o trem passou
   int i = 0;
    
   while(1){
       interEntrance(i, *(int*)threadid); // Entra na próxima intersecção
       usleep(500 * 1000);                // Espera por 500 milissegundos
       interExit(i, *(int*)threadid);     // Sai da intersecção
       i = (i < 4) ? i + 1 : 0;
   }
 
   pthread_exit(NULL);
}
 
int main(){
   // Threads e indice das Threads
   pthread_t TREM[THREADS];
   int tInd[THREADS];
 
   // Inicia as Threads
   for (int i = 0; i < THREADS; i++){
       tInd[i] = i;
       pthread_create(&TREM[i], NULL, trem, (void *)&tInd[i]);
   }
 
   pthread_exit(NULL);
}
