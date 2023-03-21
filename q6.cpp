// #INPUT DO PROGRAMA
// O codigo le a entrada do arquivo de texto q6, que segue
// o modelo:
//
// mxn -> dimensoes da matriz esparsa base do codigo 
//
// (a1,b1) (a2,b2) ... (an,bn)    os pares de inteiros e float
// (c1,d1) (c2,d2) ... (cn,dn) -> que representam os elementos
//   ...     ...   ...   ...      da matriz esparsa e seus 
// (x1,z1) (x2,z2) ... (xn,zn)    respectivos índices
//
// p -> o indice da operacao que o usuario deseja executar
//      (1-multiplicacao por vetor denso; 2-multiplicacao por
//       matriz esparsa; 3-multiplicacao por matriz densa)
// 
// #DECLARACAO DO VETOR DENSO
// Escolhida a multiplicaco por vetor denso, basta digitar
// os elementos do vetor sequencialmente, nunca excedendo o 
// limite de colunas da matriz esparsa:
//
// a1 a2 a3 ... an
//
// #DECLARACAO DA MATRIZ ESPARSA
// Escolhida a multiplicacao por matriz esparsa, o programa
// recebera o mesmo modelo previo de entrada, isto é, uma
// linha definindo as dimensoes da matriz e as linhas seguintes
// declarando os elementos da matriz:
//
// mxn -> dimensoes da matriz esparsa 
//
// (a1,b1) (a2,b2) ... (an,bn)    os pares de inteiros e float
// (c1,d1) (c2,d2) ... (cn,dn) -> que representam os elementos
//   ...     ...   ...   ...      da matriz esparsa e seus 
// (x1,z1) (x2,z2) ... (xn,zn)    respectivos índices
//
// #DECLARACAO DA MATRIZ DENSA
// Escolhida a multiplicacao por matriz densa, deve-se declarar
// as dimensoes da matriz e, nas linhas seguintes, definir seus
// elementos:
// 
// mxn -> dimensoes da matriz esparsa 
//
// a1 a2 a3 ... an
// b1 b2 b3 ... bn
// c1 c2 c3 ... cn -> elementos da matriz densa
// .. .. .. ... ..
// z1 z2 z3 ... zn
//
// OBS: Nao deve haver linhas em branco entra as linhas de
// entrada do programa.
// 
// #LOGICA DO PROGRAMA
// Apos feita a devida leitura dos dados do problema, o 
// programa inicializa a matriz onde serao armazenados os 
// resultados das operacoes e prossegue para efetuar a 
// multiplicacao dos elementos, utilizando uma thread para 
// cada linha da matriz esparsa base (por consequencia, 
// tambem cada linha da matriz resultante).
//
// Na funcao mult_vec, eh operada a multiplicacao pelo vetor 
// denso. A variavel 't' percorrera os elementos da linha da
// matriz indicada por 'tid' (o id da thread responsavel pela
// linha), armazenando em 'i' os indices de cada elemento e
// em 'num' seus valores. Eh efetuada a multiplicacao de 'num' 
// pelo elemento do vetor na posicao indicada por 'i' e o 
// resultado eh somado ao elemento da matriz resultante na 
// linha operada pela thread correspondente.
//
// Na funcao mult_spmatr, eh operada a multiplicacao pela 
// matriz esparsa. O processo inicial eh similar ao executado 
// na funcao mult_vec, porem apos atribuidos os valores de 
// 'i' e 'num1', 'i' sera passado como parametro para acessar 
// a respectiva linha da segunda matriz. Um segundo laco sera
// entao executado, perccorrendo todos os elementos da segunda 
// matriz, na linha indicada por 'i', armazenando seu indice 
// em j e seu valor em num2. Em seguida, a operacao de 
// multiplicacao eh efetuada normalmente com num1 e num2 e
// somada e armazenada na posicao indicada por tid (que 
// representa a linha operada na primeira matriz) e j (que 
// indica a coluna operada na segunda matriz) da matriz 
// resultante.
//
// Na funcao mult_dmatr, eh operada a multiplicacao pela 
// matriz densa. Similarmente ao processo executado em
// mult_vec, 't' percorrera os elementos da linha indicada, 
// armazenando os indices em 'i' e os valores em 'num', com 
// a unica excecao de que agora temos um laco a mais para 
// acessar as colunas da matriz densa. Apos executada a 
// operacao de multiplicacao, o resultado sera armazenado 
// na posicao (tid,j), que representam a linha e a coluna
// operadas, como apresentado na funcao mult_spmatr.

#include <iostream>
#include <tuple>
#include <vector>
#include <pthread.h>
using namespace std;

vector<vector<tuple<int,float>>> matrix;
vector<float> vec;
vector<vector<tuple<int,float>>> spmatr;
vector<vector<float>> dmatr;
float **result;
int resLin, resCol;
pthread_mutex_t matex = PTHREAD_MUTEX_INITIALIZER;

void *mult_vec(void *threadid) {
    int t, i;
    int tid = (*(int *)threadid);
    float num;

    for(t=0; t<matrix[tid].size(); t++) {
        i = get<0>(matrix[tid][t]);
        num = get<1>(matrix[tid][t]);
        result[tid][0] = result[tid][0] + num * vec[i];
    }

    pthread_exit(threadid);
}

void *mult_spmatr(void *threadid) {
    int t, s, i, j;
    int tid = (*(int *)threadid);
    float num1, num2;

    for(t=0;t<matrix[tid].size();t++) {
        i = get<0>(matrix[tid][t]);
        num1 = get<1>(matrix[tid][t]);
        for(s=0;s<spmatr[i].size();s++) {
            j = get<0>(spmatr[i][s]);
            num2 = get<1>(spmatr[i][s]);
            result[tid][j] = result[tid][j] + num1 * num2;
        }
    }

    pthread_exit(threadid);
}

void *mult_dmatr(void *threadid) {
    int t, i, j;
    int tid = (*(int *)threadid);
    float num;

    for(t=0;t<matrix[tid].size();t++) {
        i = get<0>(matrix[tid][t]);
        num = get<1>(matrix[tid][t]);
        for(j=0;j<resCol;j++) {
            result[tid][j] = result[tid][j] + num * dmatr[i][j]; 
        }
    }

    pthread_exit(threadid);
}

int main (int argc, char *argv[]) {
    pthread_t *threads;
    int baseLin, baseCol, opcode, pos, **threadid;
    float val;
    char last = '\0';

    scanf("%dx%d", &baseLin, &baseCol);
    for(int i=0; i<baseLin; i++) {
        vector<tuple<int,float>> vtemp;
        while(last != '\n') {
            tuple<int,float> ttemp;
            scanf(" (%d,%f)", &pos, &val);
            val = val;
            get<0>(ttemp) = pos;
            get<1>(ttemp) = val;
            vtemp.push_back(ttemp);
            scanf("%c", &last);
        }
        matrix.push_back(vtemp);
        last = '\0';
    }

    threads = new pthread_t[baseLin];
    threadid = new int*[baseLin];

    cin >> opcode;

    switch(opcode) {
        case 1: {
            float elem;
            resLin = baseLin;
            resCol = 1;
            while(last != '\n') {
                scanf(" %f%c", &elem, &last);
                vec.push_back(elem);
            }
            last = '\0';

            result = new float*[baseLin];
            for(int i=0; i<baseLin; i++) {
                result[i] = new float;
            }

            for(int i=0;i<resLin;i++) {
                result[i][0] = 0;
            }

            for(int i=0; i<baseLin; i++) {
                threadid[i] = new int;
                *threadid[i] = i;
                pthread_create(&threads[i], NULL, mult_vec, (void *)threadid[i]);
            }

            break;
        }
        case 2: {
            float elem;
            int mLin, mCol;

            scanf("%dx%d", &mLin, &mCol);
            resLin = baseLin;
            resCol = mCol;

            for(int i=0; i<mLin; i++) {
                vector<tuple<int,float>> vtemp;
                while(last != '\n') {
                    tuple<int,float> ttemp;
                    scanf(" (%d,%f)", &pos, &val);
                    val = val;
                    get<0>(ttemp) = pos;
                    get<1>(ttemp) = val;
                    vtemp.push_back(ttemp);
                    scanf("%c", &last);
                }
                spmatr.push_back(vtemp);
                last = '\0';
            }

            result = new float*[resLin];
            for(int i=0;i<resLin;i++) {
                result[i] = new float[resCol];
            }

            for(int i=0;i<resLin;i++) {
                for(int j=0;j<resCol;j++) {
                    result[i][j] = 0;
                }
            }

            for(int i=0; i<baseLin; i++) {
                threadid[i] = new int;
                *threadid[i] = i;
                pthread_create(&threads[i], NULL, mult_spmatr, (void *)threadid[i]);
            }

            break;
        }
        case 3: {
            float elem;
            int mLin, mCol;
            resLin = baseLin;

            scanf("%dx%d",&mLin,&mCol);
            resCol = mCol;

            for(int i=0; i<mLin; i++) {
                vector<float> vtemp;
                for(int j=0; j<mCol; j++) {
                    scanf(" %f", &elem);
                    vtemp.push_back(elem);
                }
                dmatr.push_back(vtemp);
            }

            result = new float*[resLin];
            for(int i=0;i<resLin;i++) {
                result[i] = new float[resCol];
            }

            for(int i=0;i<resLin;i++) {
                for(int j=0;j<resCol;j++) {
                    result[i][j] = 0;
                }
            }

            for(int i=0; i<baseLin; i++) {
                threadid[i] = new int;
                *threadid[i] = i;
                pthread_create(&threads[i], NULL, mult_dmatr, (void *)threadid[i]);
            }

            break;
        }
    }

    for(int i=0; i<baseLin; i++) {
        pthread_join(threads[i],NULL);
    }

    cout << "RESULTADO:" << endl;
    for(int j=0;j<resLin;j++) {
        for(int k=0;k<resCol;k++) {
            printf("%.1f ",result[j][k]);
        }
        cout << endl;
    }

    for(int i=0; i<matrix.size(); i++) {
        matrix[i].clear();
    }
    matrix.clear();

    vec.clear();

    for(int i=0; i<baseLin; i++) {
        delete [] result[i];
    }
    delete [] result;

    pthread_exit(NULL);
}
