#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>

using namespace std;

const int MAX_CIDADES = 100;  // tamanho max. de cidades no grafo
const int INF = INT_MAX;      // conexão inexistente, valor que representa infinito. 

//Representa uma cidade no grafo.
struct Cidade
{
    char nome[30];
    bool possuiCentroPokemon;
    int conexoes[MAX_CIDADES]; // array com um tamanho máximo da constante MAX_CIDADES que representa distâncias para outras cidades
};

Cidade cidades[MAX_CIDADES]; // Array de cidades.
int numCidades = 0; // conta as cidades que foram adicionadas no grafo.

//Função que inicializa o grafo, 
void inicializarGrafo()
{
    for (int i = 0; i < MAX_CIDADES; ++i)// linha da matriz
    {
        for (int j = 0; j < MAX_CIDADES; ++j) // coluna da matriz
        {
            cidades[i].conexoes[j] = (i == j) ? 0 : INF; // define que não há conexão direta entre i e j, caso a distancia de i não seja igual a de j (conexão pra ela mesma)
        }
    }
}

void adicionarCidade(int codigo, const char *nome, bool centroPokemon, int vizinhos[], int distancias[], int numVizinhos)
{
    if (codigo >= MAX_CIDADES)
    {
        cout << "Código de cidade excede o máximo permitido.\n";
        return;
    }
    strcpy(cidades[codigo].nome, nome); // Copia o conteúdo da string nome para o membro nome da estrutura Cidade localizada no índice codigo do array cidades
    cidades[codigo].possuiCentroPokemon = centroPokemon;

    for (int i = 0; i < numVizinhos; ++i) //Conecta a cidade às suas vizinhas, atualizando a matriz de distâncias.
    {
        int vizinho = vizinhos[i];
        int distancia = distancias[i];
        if (vizinho < MAX_CIDADES)
        {
            cidades[codigo].conexoes[vizinho] = distancia;//Distância entre a cidade atual (codigo) e o vizinho.
            cidades[vizinho].conexoes[codigo] = distancia;//Assume que o grafo é não-direcionado.
        }
    }
    numCidades = max(numCidades, codigo + 1); //atualiza o numero de cidades
}

// djikstra 
void encontrarCentroPokemonMaisProximo(int cidadeAtual)
{
    int dist[MAX_CIDADES]; //menor distância da cidade atual para cada cidade.
    bool visitado[MAX_CIDADES]; //indica se a cidade já foi visitada.
    int predecessores[MAX_CIDADES];// armazena o caminho de cada cidade.

    for (int i = 0; i < MAX_CIDADES; ++i)
    {
        dist[i] = INF; //começa com todas as distancias como INF menos a aual
        visitado[i] = false;
        predecessores[i] = -1; //inicia com -1, pois não há predecessores
    }
    dist[cidadeAtual] = 0;

    for (int i = 0; i < numCidades; ++i)
    {
        int minDist = INF, u = -1;
        for (int j = 0; j < numCidades; ++j)
        {
            if (!visitado[j] && dist[j] < minDist) //Escolhe a cidade não visitada com menor distância
            {
                minDist = dist[j];
                u = j;
            }
        } // Marca como visitada e atualiza as distancias se encontrar um caminho mais curto. 

        if (u == -1) // não há mais cidades acessiveis 
            break;
        visitado[u] = true;

        for (int v = 0; v < numCidades; ++v)
        {
            if (!visitado[v] && cidades[u].conexoes[v] != INF && dist[u] + cidades[u].conexoes[v] < dist[v])
            {
                dist[v] = dist[u] + cidades[u].conexoes[v]; // atualiza a menor distância conhecida para uma cidade vizinha (v) a partir de uma cidade atual (u)
                predecessores[v] = u;
            } //nova distância menor pelo caminho que passa por u
        }
    }

    int centroMaisProximo = -1; 
    int menorDistancia = INF; //para garantir que qualquer distancia encontrada seja menor
    for (int i = 0; i < numCidades; ++i) // percorre as cidades
    {
        if (cidades[i].possuiCentroPokemon && dist[i] < menorDistancia) // verifica se temm centro pokemon e  se a distância até a cidade atual 
            //é menor do que a menor distância encontrada até o momento.
        {
            menorDistancia = dist[i];
            centroMaisProximo = i; //Define centroMaisProximo como o índice dessa cidade.
        }
    }

    if (centroMaisProximo == -1)
    {
        cout << "Nenhum centro Pokémon acessível a partir da cidade atual.\n";
        return;
    }

    //Constroe o caminho até o centro mais próximo. 
    cout << "Rota para o centro Pokémon mais próximo (" << cidades[centroMaisProximo].nome << "):\n";
    int caminho[MAX_CIDADES]; // array armazena ss cidades do caminho na ordem inversa (do destino até a origem).
    int tamanhoCaminho = 0;
    for (int v = centroMaisProximo; v != -1; v = predecessores[v]) // para voltar uma cidade por vez até a origem (cidade atual, onde o caminho começou).
    {
        caminho[tamanhoCaminho++] = v;
    }

    for (int i = tamanhoCaminho - 1; i >= 0; --i) //percorre o array do inicio até o destino
    {
        cout << cidades[caminho[i]].nome; //
        if (i > 0)
            cout << " -> ";
    }
    cout << "\nDistância total: " << menorDistancia << "\n"; //Mostra a menor distância total até o centro Pokémon mais próximo.
}
