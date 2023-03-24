// Em C++
#include <pthread.h>
#include <iostream>
#include <vector>
#include <list>
#define INFINITY 1000
using namespace std;
class Floresta{
public:
  int n_arvores;
  vector<int> pai;
  vector<int> rank;
  vector<list<int>> arvore;
  Floresta(int n_nodes){
      n_arvores = n_nodes;
      pai.resize(n_nodes);
      rank.resize(n_nodes);
      arvore.resize(n_nodes);
      for (int i = 0; i < n_nodes; i++){
          pai[i] = i;
          rank[i] = 0;
          arvore[i].push_back(i);
      }
  }
  int Find(int node){
      if (pai[node] == node)
          return pai[node];
      int raiz = Find(pai[node]);
      pai[node] = raiz;
      return raiz;
  }
  void Union(int node1, int node2){
      int raiz1 = Find(node1);
      int raiz2 = Find(node2);
      if (rank[raiz1] > rank[raiz2])
          swap(raiz1, raiz2);
      if (rank[raiz1] == rank[raiz2])
          rank[raiz2]++;
      arvore[raiz2].splice(arvore[raiz2].end(), arvore[raiz1]);
      pai[raiz1] = pai[raiz2];
      this->n_arvores -= 1;
        }
};
struct aresta{
  int node1;
  int node2;
  int peso;
  aresta(int p){
      peso = p;
  }
  aresta(int n1, int n2, int p){
      node1 = n1;
      node2 = n2;
      peso = p;
  }
};
pthread_mutex_t treeMutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para uso do Find (altera valores da Floresta)
vector<vector<pair<int, int>>> grafo = {};             // Armazena o gráfico testado (definido no main)
Floresta f(0);                                         // Armazena a floresta que está sendo gerada
void *bestEdgeThreadFinder(void *args){
  // Inicializa a melhor aresta com o peso infinito
  struct aresta* best_edge = (struct aresta*)malloc(sizeof(struct aresta));
  *best_edge = aresta(INFINITY);
  int raiz = *(int*)args; // Indice da raiz da árvore
  // Referencia a árvore cuja raiz é o indice passado pra thread
  list<int> arvore = f.arvore[raiz];
  list<int>::iterator aInd = arvore.begin();
  int num = (int)arvore.size();
  // Testa para todos os vértices da árvore
  for (int i = 0; i < num; i++){
      // Testa para todas as arestas que saem dele
      for (auto a : grafo[*aInd]){
          // Armazena o vértice vizinho e o peso que os liga
          int vizinho = a.first;
          int peso_node_vizinho = a.second;
          // Filtra apenas as arestas que ligam a arvore a outra árvore
          pthread_mutex_lock(&treeMutex); // Trava o mutex para o uso do Find()
          if (f.Find(vizinho) == f.Find(*aInd))
              continue;
                        pthread_mutex_unlock(&treeMutex);
          // Checa se o peso é menor do que o melhor peso atual
          if ((*best_edge).peso > peso_node_vizinho)
              *best_edge = aresta(*aInd, vizinho, peso_node_vizinho);
      }
  }
  // Retorna a melhor aresta
  pthread_exit((void *)best_edge);
}
vector<aresta> find_best_edges(vector<aresta> &mst){
  int n_nodes = grafo.size();
  vector<aresta> best_edges;
  // Máximo de n_nodes threads (quando cada vértice é uma árvore)
  pthread_t threads[n_nodes];
  int inds[n_nodes], tCount = 0;
  for (int node = 0; node < n_nodes; node++){
      // Achando raiz que representa a árvore
      if (f.pai[node] == node){
          // Cria uma thread pra cada árvore para buscar a melhor aresta que liga ela com outra árvore
          int raiz = node;
          inds[tCount] = node;
          pthread_create(&threads[tCount], NULL, bestEdgeThreadFinder, (void *)&inds[tCount]);
          tCount++;
      }
  }
  // Recebe o retorno das threads, contendo a melhor aresta de cada ávore
  aresta *res;
  for(int i = 0; i < tCount; i++){
      pthread_join(threads[i], (void**)&res);
      best_edges.push_back(*res);
  }
  // Retorna o vetor com as melhores arestas
  return best_edges;
}
vector<aresta> boruvka(){
  vector<aresta> mst;
  int n_nodes = grafo.size();
  // Inicializa conjunto de arvores (Floresta)
  f = Floresta(n_nodes);
  // Repete enquanto houver mais de uma árvore
    while (f.n_arvores > 1){
      // Achar a melhor aresta de cada árvore
      vector<aresta> best_edges = find_best_edges(mst);
      // Realiza a união das árvores a partir das melhores arestas
      for (auto best_edge : best_edges){
          int n1 = best_edge.node1;
          int n2 = best_edge.node2;
          // Checa se as árvores já não estão juntas
          if (f.Find(n1) != f.Find(n2)){
              mst.push_back(best_edge);
              f.Union(n1, n2);
          }
      }
  }
  return mst;
}
int main(){
  // Rodar o código: gcc -pthread q4.cpp -lstdc++ -o exe.o
  // Montar o gráfico, cada linha é o conjunto das arestas de um vértice na forma (segundo_vértice, peso)
 
  //  Grafo 1:
  //      2   3
  //  (0)--(1)--(2)
  //   |       / |
  //  6|   _8/   |7
  //   | /       |
  //  (3)-------(4)
  //        9
  vector<vector<pair<int, int>>> graph1 = {
      {make_pair(1, 2), make_pair(3, 6)},                  // Vertice 0
      {make_pair(0, 2), make_pair(2, 3)},                  // Vertice 1
      {make_pair(1, 3), make_pair(3, 8), make_pair(4, 7)}, // Vertice 2
      {make_pair(0, 6), make_pair(2, 8), make_pair(4, 9)}, // Vertice 3
      {make_pair(3, 9), make_pair(2, 7)}                   // Vertice 4
  };
 
  //  Grafo 2:
  //      2   1
  //  (0)--(1)--(2)
  //   |  \      | \4
  //  9|   3\   7|  (5)
  //   |      \  | /3
    //  (3)-------(4)
  //       6
  vector<vector<pair<int, int>>> graph2 = {
      {make_pair(1, 2), make_pair(3, 9), make_pair(4, 3)},                  // 0
      {make_pair(0, 2), make_pair(2, 1)},                                   // 1
      {make_pair(1, 1), make_pair(4, 7), make_pair(5, 4)},                  // 2
      {make_pair(0, 9), make_pair(4, 6)},                                   // 3
      {make_pair(3, 6), make_pair(0, 3), make_pair(2, 7), make_pair(5, 3)}, // 4
      {make_pair(2, 4), make_pair(4, 3)}                                    // 5
  };
 
   // Escolher um dos 2 exemplos de grafo preparados acima
   grafo = graph1;
   vector<aresta> mst = boruvka();
 
   // Exibe as arestas da árvore
   for (auto a : mst)
       cout << a.node1 << " - " << a.node2 << " " << a.peso << endl;
}
