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