#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Lib do sleep
 
#define DBSize 5 // Tamanho do Banco de Dados
#define QntR 2 // Quantidade de Threads leitoras
#define QntW 2 // Quantidade de Threads escritoras
 
int database[DBSize] = { 0 }; // Banco de Dados inicia com todos os valores em 0
pthread_mutex_t dbMutex[DBSize] = { PTHREAD_MUTEX_INITIALIZER }; // Bloquear cada item do Banco de Dados separadamente
pthread_cond_t writeCond = PTHREAD_COND_INITIALIZER; // Condição para poder escrever (não há mais leitores lendo a Database)
int readers[DBSize] = { 0 }; // Contador de leitores atualmente lendo em cada posição do Banco de Dados
 
void* leitor(void *arg){
   // Indice da Thread
   int *tInd = (int*)arg;
 
   while(1){
       // Sorteia uma posição do Banco de Dados
       int r = (int)(random() % DBSize);
 
       // Locka o Mutex para incrementar o contador de leitores lendo a Database
       pthread_mutex_lock(&dbMutex[r]);
       readers[r]++;
       pthread_mutex_unlock(&dbMutex[r]);
 
       // Lê o valor na posição sorteada
       printf("Li o valor %d na posição %d (%d)\n", database[r], r, *tInd);
 
       // Locka o Mutex para decrementar o contador atual de leitores
       pthread_mutex_lock(&dbMutex[r]);
       readers[r]--;
       // Se não houver mais leitores, sinaliza as threads escritoras esperando para escrever

       if(readers[r] == 0)
           pthread_cond_signal(&writeCond);
       pthread_mutex_unlock(&dbMutex[r]);
       sleep(1);
   }
}
 
void* escritor(void *arg){
   // Indice da Thread
   int *tInd = (int*)arg;
 
   while(1){
       // Sorteia uma posição do Banco de Dados
       int r = (int)(random() % DBSize);
 
       // Locka o Mutex para escrever no Banco de Dados
       pthread_mutex_lock(&dbMutex[r]);
       // Se houver alguma thread lendo o Banco de Dados, espera até que não tenha mais
       while(readers[r] > 0)
           pthread_cond_wait(&writeCond, &dbMutex[r]);
 
       // Escreve um valor aleatório na posição sorteada
       int val = (int)(random() % 100);
       database[r] = val;
       printf("Escrevi o valor %d na posição %d (%d)\n", val, r, *tInd);
 
       // Sinaliza para outras threads escritoras que estavam no wait além dela
       pthread_cond_signal(&writeCond);
       pthread_mutex_unlock(&dbMutex[r]);
       sleep(1);
   }
}
 
int main(){
   // Número de leitores (n) e número de escritores (m)
   int n = QntR, m = QntW, a[m], b[n];
   pthread_t leitores[n], escritores[m];
 
   // Cria as Threads dos escritores
   for(int i = 0; i < m; i++){
           a[i] = i;
       pthread_create(&escritores[i], NULL, escritor, (void*)&a[i]);
   }
 
   // Cria as Threads dos leitores
   for(int i = 0; i < n; i++){
       b[i] = i;
       pthread_create(&leitores[i], NULL, leitor, (void*)&b[i]);
   }
 
   // Usa o Join nas threads
   for(int i = 0; i < n; i++)
       pthread_join(leitores[i], NULL);
   for(int i = 0; i < m; i++)
       pthread_join(escritores[i], NULL);
}

