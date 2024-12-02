#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>

using namespace std;

const int MAX_CIDADES = 100;  // tamanho max. de cidades no grafo
const int INF = INT_MAX;      // conexão inexistente

struct Cidade
{
    char nome[30];
    bool possuiCentroPokemon;
    int conexoes[MAX_CIDADES]; // distâncias para outras cidades
};

Cidade cidades[MAX_CIDADES];
int numCidades = 0;


void inicializarGrafo()
{
    for (int i = 0; i < MAX_CIDADES; ++i)
    {
        for (int j = 0; j < MAX_CIDADES; ++j)
        {
            cidades[i].conexoes[j] = (i == j) ? 0 : INF;
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
    strcpy(cidades[codigo].nome, nome);
    cidades[codigo].possuiCentroPokemon = centroPokemon;

    for (int i = 0; i < numVizinhos; ++i)
    {
        int vizinho = vizinhos[i];
        int distancia = distancias[i];
        if (vizinho < MAX_CIDADES)
        {
            cidades[codigo].conexoes[vizinho] = distancia;
            cidades[vizinho].conexoes[codigo] = distancia;
        }
    }
    numCidades = max(numCidades, codigo + 1);
}

// djikstra
void encontrarCentroPokemonMaisProximo(int cidadeAtual)
{
    int dist[MAX_CIDADES];
    bool visitado[MAX_CIDADES];
    int predecessores[MAX_CIDADES];

    for (int i = 0; i < MAX_CIDADES; ++i)
    {
        dist[i] = INF;
        visitado[i] = false;
        predecessores[i] = -1;
    }
    dist[cidadeAtual] = 0;

    for (int i = 0; i < numCidades; ++i)
    {
        int minDist = INF, u = -1;
        for (int j = 0; j < numCidades; ++j)
        {
            if (!visitado[j] && dist[j] < minDist)
            {
                minDist = dist[j];
                u = j;
            }
        }

        if (u == -1)
            break;
        visitado[u] = true;

        for (int v = 0; v < numCidades; ++v)
        {
            if (!visitado[v] && cidades[u].conexoes[v] != INF && dist[u] + cidades[u].conexoes[v] < dist[v])
            {
                dist[v] = dist[u] + cidades[u].conexoes[v];
                predecessores[v] = u;
            }
        }
    }

    int centroMaisProximo = -1;
    int menorDistancia = INF;
    for (int i = 0; i < numCidades; ++i)
    {
        if (cidades[i].possuiCentroPokemon && dist[i] < menorDistancia)
        {
            menorDistancia = dist[i];
            centroMaisProximo = i;
        }
    }

    if (centroMaisProximo == -1)
    {
        cout << "Nenhum centro Pokémon acessível a partir da cidade atual.\n";
        return;
    }

    cout << "Rota para o centro Pokémon mais próximo (" << cidades[centroMaisProximo].nome << "):\n";
    int caminho[MAX_CIDADES];
    int tamanhoCaminho = 0;
    for (int v = centroMaisProximo; v != -1; v = predecessores[v])
    {
        caminho[tamanhoCaminho++] = v;
    }

    for (int i = tamanhoCaminho - 1; i >= 0; --i)
    {
        cout << cidades[caminho[i]].nome;
        if (i > 0)
            cout << " -> ";
    }
    cout << "\nDistância total: " << menorDistancia << "\n";
}