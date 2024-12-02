#include <iostream>
#include <climits>
#include <math.h>
#include <stack>
#include <algorithm>

using namespace std;

const int INF = INT_MAX;      // conexão inexistente
const int MAX_POKEMONS = 100; // tamanho max. de pokemons na pokedex

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

//INSERÇÃO COM BALANCEAMENTO
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

