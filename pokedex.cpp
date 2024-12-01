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

bool compare(Pokemon p1, Pokemon p2)
{
    int o = orientation(p0, p1, p2);
    if (o == 0)
        return distSq(p0, p2) >= distSq(p0, p1);
    return (o == 2);
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
    setlocale(LC_ALL, "pt_BR.UTF-8");

    Node *raiz = NULL;
    int opcao = -1;

    inicializarGrafo();
    while (true)
    {
        cout << "---------------Bem-vindo, treinador!------------------" << endl;
        cout << "Digite 1 para operações com cidades | 2 para operações com Pokémons | 0 para sair" << endl;
        cin >> opcao;
        if (opcao == 1)
        {
            int op;
            cout << "---------------MAPA----------------" << endl;
            do
            {
                cout << "Digite 1 para adicionar uma cidade e 2 para encontrar um caminho | 0 para encerrar" << endl;
                cin >> op;
                switch (op)
                {
                case 1:
                {
                    cout << "Quantas cidades deseja adicionar? ";
                    int num;
                    cin >> num;
                    for (int i = 0; i < num; ++i)
                    {
                        int codigo;
                        char nome[30];
                        bool centroPokemon;
                        int numVizinhos;
                        cout << "\nDigite o código da cidade: ";
                        cin >> codigo;
                        cin.ignore();
                        cout << "Digite o nome da cidade: ";
                        cin.getline(nome, 30);
                        cout << "A cidade possui centro Pokémon? (Digite 1 para SIM ou digite 0 para NÂO): ";
                        cin >> centroPokemon;
                        cout << "Quantas cidades vizinhas? ";
                        cin >> numVizinhos;
                        int vizinhos[MAX_CIDADES];
                        int distancias[MAX_CIDADES];
                        cout << endl;
                        cout << "Digite os códigos das cidades vizinhas e as distâncias:\n";
                        for (int j = 0; j < numVizinhos; ++j)
                        {
                            cout << "Código da cidade vizinha: ";
                            cin >> vizinhos[j];
                            cout << "Distância até essa cidade: ";
                            cin >> distancias[j];
                        }
                        adicionarCidade(codigo, nome, centroPokemon, vizinhos, distancias, numVizinhos);
                    }
                    break;
                }
                case 2:
                {
                    cout << "\nDigite o código da cidade atual para buscar o centro Pokémon mais próximo: ";
                    int cidadeAtual;
                    cin >> cidadeAtual;
                    encontrarCentroPokemonMaisProximo(cidadeAtual);
                }
                }
            } while (op != 0);
        }
        else if (opcao == 2)
        {
            int op;
            cout << "---------------POKÉDEX----------------" << endl;
            while (true)
            {
                cout << "1. Inserir Pokemon \n2. Imprimir Pokemons em ordem alfabetica\n3. Verificar Pokemon por nome\n4. Imprimir Pokemons por tipo\n5. Contar Pokemons por tipo\n 6.Contar Pokemons por raio \n 7. Perimetro minimo para achar os pokemons \n0. Sair\n";
                cin >> op;
                if (op == 0)
                {
                    break;
                }
                if (op == 1)
                {
                    string nome, tipo;
                    int numero, x, y;
                    cout << "Nome: ";
                    cin >> nome;
                    cout << "Tipo: ";
                    cin >> tipo;
                    cout << "Numero: ";
                    cin >> numero;
                    cout << "Localizacao (x, y): ";
                    cin >> x >> y;
                    Pokemon p(nome, tipo, numero, x, y);
                    raiz = inserirPokemon(raiz, p);
                }
                else if (op == 2)
                {
                    imprimirEmOrdem(raiz);
                }
                else if (op == 3)
                {
                    string nome;
                    cout << "Nome do Pokemon: ";
                    cin >> nome;
                    if (buscarPokemon(raiz, nome))
                    {
                        cout << "Pokemon encontrado!\n";
                    }
                    else
                    {
                        cout << "Pokemon nao encontrado!\n";
                    }
                }
                else if (op == 4)
                {
                    imprimirPokemonTipo(raiz);
                }
                else if (op == 5)
                {
                    string tipo;
                    cout << "Tipo do Pokemon: ";
                    cin >> tipo;
                    int count = contarPokemonTipo(raiz, tipo);
                    cout << "Numero de Pokemons do tipo " << tipo << ": " << count << "\n";
                }
                else if (op == 6)
                {
                    int x, y;
                    cout << "Digite a localização atual (x y): ";
                    cin >> x >> y;
                    int raio = 100; // raio de 100 metros
                    int quantidade = contarPokemonNoRaio(raiz, x, y, raio);
                    cout << "Número de Pokémons encontrados dentro de 100 metros: " << quantidade << endl;
                }
                else if (op == 7)
                {
                    Pokemon pokemons[MAX_POKEMONS];
                    int index = 0;

                    coletarPokemons(raiz, pokemons, index);

                    Pokemon fechoConvexo[MAX_POKEMONS];
                    int size = 0;

                    grahamScan(pokemons, index, fechoConvexo, size);

                    double perimetro = calcularPerimetro(fechoConvexo, size);
                    cout << "Todos os pokemons se encontram no perimetro de: " << perimetro << " metros"<< endl;
                }
                else
                {
                    cout << "Opção inválida!\n";
                }
            }
        }
        else
        {
            break;
        }
    }

    destruirArvore(raiz);
    return 0;
}