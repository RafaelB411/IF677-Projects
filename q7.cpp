// As threads verificam a ocorrencia de elementos repetidos
// nas linhas e colunas da matriz 'M' paralelamente, alterando 
// o valor da variavel 'latino' para false caso seja identificada
// a ocorrência de um elemento repetido numa linha ou coluna
// específica. A variavel 'latino' representa o status da matriz
// sendo um quadrado latino ou nao. Ao final do programa, se
// a variavel tiver o valor "TRUE" atribuído, a matriz eh um 
// quadado latino. Caso contrário, ela nao eh.

// g++ -pthread q7.cpp -o q7
// ./q7

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define LIN 3
#define COL 3

bool latino = true;
char M[LIN][COL] = {{'a','b','c'},
                    {'2','c','b'},
                    {'3','2','a'}};

void* checkLine(void *tid){
    int i,j,k;
    char key;

    for(int i=0;i<LIN;i++) {
        for(int j=0;j<COL;j++) {
            key = M[i][j];
            for(int k=i+1;k<LIN;k++) {
                if(key==M[k][j]) {
                    latino = false;
                }
            }
        }
    }
    pthread_exit(NULL);
}

void* checkColumn(void *tid){
    int i,j,k;
    char key;

    for(int i=0;i<LIN;i++) {
        for(int j=0;j<COL;j++) {
            key = M[i][j];
            for(int k=j+1;k<COL;k++) {
                if(key==M[i][k]) {
                    latino = false;
                }
            }
        }
    }
    pthread_exit(NULL);
}

int main (int argc, char *argv[]){
    pthread_t threadL, threadC;
    int i,j,rc;

    pthread_create(&threadL,NULL,checkLine,NULL);
    pthread_create(&threadC,NULL,checkColumn,NULL);

    pthread_join(threadL,NULL);
    pthread_join(threadC,NULL);

    if(latino) { printf("A matriz eh um quadrado latino\n"); }
    else { printf("A matriz nao eh um quadrado latino\n"); }

    pthread_exit(NULL);
}