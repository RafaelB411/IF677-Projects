#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Lib do sleep
 
#define Fome 1
#define Pensando 2
#define Comendo 3
 
pthread_mutex_t monitorMutex = PTHREAD_MUTEX_INITIALIZER; // Mutex pra usar os metodos askAccess() e releaseAccess()
pthread_cond_t fredFork[5] = { PTHREAD_COND_INITIALIZER };// Aviso que pode comer
int forkState[5] = {1, 1, 1, 1, 1};                       // Estado dos Garfos (1 = livre, 0 = ocupado)
int filState[5] = { Pensando };                           // Estado de cada Filósofo
int priority[5] = { 0 };                                  // Prioridade de cada Filósofo
 
void adjustPriorityArray(){
   // Impede os valores da lista de prioridade de diminuirem infinitamente
   int cont = 0;
   for(int i = 0; i < 5; i++)
       if(priority[i] < 0)
           cont++;
   if(cont == 5)
       for(int i = 0; i < 5; i++)
           priority[i]++;
}
 
void releaseAccess(int tInd){
   // Indice do segundo garfo
   int leftFork = (tInd < 4) ? tInd + 1 : 0, prevFil = (tInd > 0) ? tInd - 1 : 4;
   // Muda o estado dos garfos para livre
   forkState[tInd] = forkState[leftFork] = 1;
 
   // Muda estado do filósofo para Pensando, e diminui em um a prioridade dele
      filState[tInd] = Pensando;
   priority[tInd]--;
   adjustPriorityArray();
   // Avisa os filosofos adjacentes
   if(filState[prevFil] == Fome)
       pthread_cond_signal(&fredFork[prevFil]);
   if(filState[leftFork] == Fome)
       pthread_cond_signal(&fredFork[leftFork]);
}
int askAccess(int tInd){
   // Indice do segundo garfo
   int leftFork = (tInd < 4) ? tInd + 1 : 0, prevFil = (tInd > 0) ? tInd - 1 : 4;
   // Checa se os garfos estão ocupados ou se a vez é de outro filósofo
   if(forkState[tInd] != 1 || forkState[leftFork] != 1 ||
      (filState[leftFork] == Fome && priority[leftFork] > priority[tInd]) ||
      (filState[prevFil] == Fome && priority[prevFil] > priority[tInd])){
       // O filósofo espera pra comer
       filState[tInd] = Fome;
       return 0;
   }
   else{
       // O filósofo começa a comer
       forkState[tInd] = forkState[leftFork] = 0;
       filState[tInd] = Comendo;
       return 1;
   }
}
 
void think(int tInd){
   // Pensa por 1 a 5 segundos
   printf("O filosofo %d esta pensando\n", tInd+1);
   sleep((random() % 4) + 1);
}
 
void get_forks(int tInd){
   // Tenta pegar os garfos, pedindo acesso ao método monitor, esperando caso falhe
   pthread_mutex_lock(&monitorMutex);
   printf("O filosofo %d ficou com fome\n", tInd+1);
   while (askAccess(tInd) == 0){
              pthread_cond_wait(&fredFork[tInd], &monitorMutex);
   }
   pthread_mutex_unlock(&monitorMutex);
}
 
void eat(int tInd){
   // Come por 1 a 5 segundos
   printf("O filosofo %d esta comendo\n", tInd+1);
   sleep((random() % 4) + 1);
}
 
void put_forks(int tInd){
   // Devolve os garfos pelo método monitor
   pthread_mutex_lock(&monitorMutex);
   releaseAccess(tInd);
   pthread_mutex_unlock(&monitorMutex);
   printf("O filosofo %d parou de comer\n", tInd+1);
}
void *filosofo(void *arg){
   // Indice dos garfos
   int tInd = *(int *)arg, leftFork = (tInd < 4) ? tInd + 1 : 0;
   while (1){
       think(tInd);
       get_forks(tInd);
       eat(tInd);
       put_forks(tInd);
   }
}
int main(){
   // Threads e indice das Threads
   pthread_t filosofos[5];
   int tInd[5];
   // Cria as Threads dos filosofos
   for (int i = 0; i < 5; i++){
       tInd[i] = i;
       pthread_create(&filosofos[i], NULL, filosofo, (void *)&tInd[i]);
   }
   for (int i = 0; i < 5; i++)
       pthread_join(filosofos[i], NULL);
}
