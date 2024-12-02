#include <iostream>
#include <cstring>
#include <climits>
#include <locale>
#include <math.h>
#include <stack>
#include <vector>
#include <algorithm>
#include <stdlib.h>

using namespace std;

const int MAX_CIDADES = 100;  // tamanho max. de cidades no grafo
const int INF = INT_MAX;      // conexão inexistente
const int MAX_POKEMONS = 100; // tamanho max. de pokemons na pokedex

struct Cidade
{
    char nome[30];
    bool possuiCentroPokemon;
    int conexoes[MAX_CIDADES]; // distâncias para outras cidades
};

Cidade cidades[MAX_CIDADES];
int numCidades = 0;

struct Pokemon
{
    string nome;
    string tipo;
    int numero;
    int x, y;

    Pokemon() {}
    Pokemon(string n, string t, int num, int px, int py)
        : nome(n), tipo(t), numero(num), x(px), y(py) {}
};

Pokemon p0;

struct Node
{
    Pokemon pokemon;
    Node *esquerda;
    Node *direita;

    Node(Pokemon p) : pokemon(p), esquerda(NULL), direita(NULL) {}
};

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

int altura(Node *node)
{
    if (node == nullptr)
        return 0;
    return max(altura(node->esquerda), altura(node->direita)) + 1;
}

int fatorBalanceamento(Node *node)
{
    if (node == nullptr)
        return 0;
    return altura(node->esquerda) - altura(node->direita);
}

Node *rotacaoDireita(Node *y)
{
    Node *x = y->esquerda;
    Node *T2 = x->direita;

    x->direita = y;
    y->esquerda = T2;

    return x;
}

Node *rotacaoEsquerda(Node *x)
{
    Node *y = x->direita;
    Node *T2 = y->esquerda;

    y->esquerda = x;
    x->direita = T2;

    return y;
}

Node *inserirPokemon(Node *raiz, Pokemon p)
{
    if (raiz == nullptr)
    {
        return new Node(p);
    }

    if (p.nome < raiz->pokemon.nome)
    {
        raiz->esquerda = inserirPokemon(raiz->esquerda, p);
    }
    else if (p.nome > raiz->pokemon.nome)
    {
        raiz->direita = inserirPokemon(raiz->direita, p);
    }

    int fb = fatorBalanceamento(raiz);

    if (fb > 1 && p.nome < raiz->esquerda->pokemon.nome)
        return rotacaoDireita(raiz);

    if (fb < -1 && p.nome > raiz->direita->pokemon.nome)
        return rotacaoEsquerda(raiz);

    if (fb > 1 && p.nome > raiz->esquerda->pokemon.nome)
    {
        raiz->esquerda = rotacaoEsquerda(raiz->esquerda);
        return rotacaoDireita(raiz);
    }

    if (fb < -1 && p.nome < raiz->direita->pokemon.nome)
    {
        raiz->direita = rotacaoDireita(raiz->direita);
        return rotacaoEsquerda(raiz);
    }

    return raiz;
}

bool buscarPokemon(Node *raiz, string nome)
{
    if (raiz == NULL)
    {
        return false;
    }
    if (raiz->pokemon.nome == nome)
    {
        return true;
    }
    if (nome < raiz->pokemon.nome)
    {
        return buscarPokemon(raiz->esquerda, nome);
    }
    else
    {
        return buscarPokemon(raiz->direita, nome);
    }
}

void imprimirEmOrdem(Node *raiz)
{
    if (raiz != NULL)
    {
        imprimirEmOrdem(raiz->esquerda);
        cout << "Nome: " << raiz->pokemon.nome
             << " - Tipo: " << raiz->pokemon.tipo
             << " - Numero: " << raiz->pokemon.numero
             << " - Localizacao: (" << raiz->pokemon.x << ", " << raiz->pokemon.y << ")\n";
        imprimirEmOrdem(raiz->direita);
    }
}

void coletarPokemons(Node *raiz, Pokemon pokemons[], int &index)
{
    if (raiz != NULL)
    {
        coletarPokemons(raiz->esquerda, pokemons, index);
        pokemons[index++] = raiz->pokemon;
        coletarPokemons(raiz->direita, pokemons, index);
    }
}

void ordenarPokemonTipo(Pokemon pokemons[], int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = 0; j < n - i - 1; ++j)
        {
            if (pokemons[j].tipo > pokemons[j + 1].tipo)
            {
                Pokemon temp = pokemons[j];
                pokemons[j] = pokemons[j + 1];
                pokemons[j + 1] = temp;
            }
        }
    }
}

void imprimirPokemonTipo(Node *raiz)
{
    if (raiz == NULL)
        return;

    const int MAX_POKEMONS = 100;
    Pokemon pokemons[MAX_POKEMONS];
    int index = 0;

    coletarPokemons(raiz, pokemons, index);
    ordenarPokemonTipo(pokemons, index);

    for (int i = 0; i < index; ++i)
    {
        cout << "Nome: " << pokemons[i].nome
             << " - Tipo: " << pokemons[i].tipo
             << " - Numero: " << pokemons[i].numero
             << " - Localizacao: (" << pokemons[i].x << ", " << pokemons[i].y << ")\n";
    }
}

int contarPokemonTipo(Node *raiz, string tipo)
{
    if (raiz == NULL)
    {
        return 0;
    }
    int count = 0;
    if (raiz->pokemon.tipo == tipo)
    {
        count = 1;
    }
    return count + contarPokemonTipo(raiz->esquerda, tipo) + contarPokemonTipo(raiz->direita, tipo);
}

double calcularDistancia(int x1, int y1, int x2, int y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int contarPokemonNoRaio(Node *raiz, int x, int y, int raio)
{
    if (raiz == NULL)
    {
        return 0;
    }

    int count = 0;
    double distancia = calcularDistancia(raiz->pokemon.x, raiz->pokemon.y, x, y);

    if (distancia <= raio)
    {
        count = 1;
    }

    return count + contarPokemonNoRaio(raiz->esquerda, x, y, raio) + contarPokemonNoRaio(raiz->direita, x, y, raio);
}

void destruirArvore(Node *raiz)
{
    if (raiz != NULL)
    {
        destruirArvore(raiz->esquerda);
        destruirArvore(raiz->direita);
        delete raiz;
    }
}

Pokemon nextToTop(stack<Pokemon> &S)
{
    Pokemon p = S.top();
    S.pop();
    Pokemon res = S.top();
    S.push(p);
    return res;
}

int distSq(Pokemon p1, Pokemon p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) +
           (p1.y - p2.y) * (p1.y - p2.y);
}

int orientation(Pokemon p, Pokemon q, Pokemon r)
{
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0)
        return 0;             // colinear
    return (val > 0) ? 1 : 2; // horário ou anti-horário
}

void grahamScan(Pokemon pokemons[], int n, Pokemon resultado[], int &size)
{
    if (n < 3)
    {
        cout << "Não é possível formar um fecho convexo com menos de 3 pontos." << endl;
        return;
    }

    int ymin = pokemons[0].y, min = 0;
    for (int i = 1; i < n; i++)
    {
        if (pokemons[i].y < ymin || (pokemons[i].y == ymin && pokemons[i].x < pokemons[min].x))
        {
            ymin = pokemons[i].y;
            min = i;
        }
    }

    stack<Pokemon> S;
    S.push(pokemons[0]);
    S.push(pokemons[1]);
    S.push(pokemons[2]);

    for (int i = 3; i < n; i++)
    {
        while (S.size() > 1 && orientation(nextToTop(S), S.top(), pokemons[i]) != 2)
            S.pop();
        S.push(pokemons[i]);
    }

    size = 0;
    while (!S.empty())
    {
        resultado[size] = S.top();
        S.pop();
        size++;
    }
}

double distPokemon(Pokemon p1, Pokemon p2)
{
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

double calcularPerimetro(Pokemon pokemons[], int n)
{
    Pokemon fechoConvexo[MAX_POKEMONS];
    int size = 0;
    grahamScan(pokemons, n, fechoConvexo, size);

    double perimetro = 0.0;
    for (int i = 0; i < size; i++)
    {
        perimetro += distPokemon(fechoConvexo[i], fechoConvexo[(i + 1) % size]);
    }

    return perimetro;
}

int main()
{

    inicializarGrafo();

    int vizinhos[] = {1};
    int distancias[] = {10};
    adicionarCidade(0, "New Bark Town", false, vizinhos, distancias, 1);

    int vizinhos2[] = {0, 2};
    int distancias2[] = {10, 20};
    adicionarCidade(1, "Cherrygrove City", false, vizinhos2, distancias2, 2);

    int vizinhos3[] = {1, 3};
    int distancias3[] = {20, 30};
    adicionarCidade(2, "Violet City", false, vizinhos3, distancias3, 2);

    int vizinhos4[] = {2, 4};
    int distancias4[] = {30, 25};
    adicionarCidade(3, "Azalea Town", false, vizinhos4, distancias4, 2);

    int vizinhos5[] = {3, 5};
    int distancias5[] = {25, 15};
    adicionarCidade(4, "Goldenrod City", true, vizinhos5, distancias5, 2);

    int vizinhos6[] = {4, 6};
    int distancias6[] = {15, 20};
    adicionarCidade(5, "Ecruteak City", false, vizinhos6, distancias6, 2);

    int vizinhos7[] = {5};
    int distancias7[] = {20};
    adicionarCidade(6, "Olivine City", true, vizinhos7, distancias7, 1);

    int vizinhos8[] = {6, 8};
    int distancias8[] = {25, 30};
    adicionarCidade(7, "Cianwood City", false, vizinhos8, distancias8, 2);

    int vizinhos9[] = {7, 9};
    int distancias9[] = {30, 35};
    adicionarCidade(8, "Mahogany Town", false, vizinhos9, distancias9, 2);

    int vizinhos10[] = {8, 10};
    int distancias10[] = {35, 40};
    adicionarCidade(9, "Blackthorn City", true, vizinhos10, distancias10, 2);

    int vizinhos11[] = {9};
    int distancias11[] = {40};
    adicionarCidade(10, "Mt. Silver", false, vizinhos11, distancias11, 1);

    int cidadeAtual = 2;
    cout << "Cidade Atual: " << cidades[cidadeAtual].nome << endl;

    encontrarCentroPokemonMaisProximo(cidadeAtual);

    Pokemon pikachu("Pikachu", "Elétrico", 25, 10, 20);
    Pokemon charmander("Charmander", "Fogo", 4, 15, 10);
    Pokemon bulbasaur("Bulbasaur", "Grama", 1, 5, 8);
    Pokemon squirtle("Squirtle", "Água", 7, 12, 15);

    Node *raiz = NULL;
    raiz = inserirPokemon(raiz, pikachu);
    raiz = inserirPokemon(raiz, charmander);
    raiz = inserirPokemon(raiz, bulbasaur);
    raiz = inserirPokemon(raiz, squirtle);

    cout << "\nPokémons na Pokédex em ordem alfabética:\n";
    imprimirEmOrdem(raiz);

    cout << "\nQuantidade de Pokémons do tipo fogo: " << contarPokemonTipo(raiz, "Fogo") << endl;

    cout << "\nPokémons no raio de 100 metros da posição (10, 10): " << contarPokemonNoRaio(raiz, 10, 10, 100) << endl;

    Pokemon pokemons[MAX_POKEMONS];
    int index = 0;
    coletarPokemons(raiz, pokemons, index);

    double perimetro = calcularPerimetro(pokemons, index);
    cout << "\nPerímetro mínimo para capturar todos os Pokémons: " << perimetro << " metros" << endl;

    destruirArvore(raiz);
    return 0;
}
