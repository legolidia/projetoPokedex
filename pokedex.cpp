#include <iostream>
#include <cstring>
#include <climits>
#include <locale>

using namespace std;

const int MAX_CIDADES = 100; // Define o tamanho máximo de cidades no grafo
const int INF = INT_MAX;     // Conexão inexistente ou caminho impossível de alcançar no grafo

// Armazenar dados de cada cidade
struct Cidade
{
    char nome[30];
    bool possuiCentroPokemon;
    int conexoes[MAX_CIDADES]; // Armazena as dist�ncias para outras cidades
};

Cidade cidades[MAX_CIDADES]; // Array de cidades
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

struct Node
{
    Pokemon pokemon;
    Node *esquerda;
    Node *direita;

    Node(Pokemon p) : pokemon(p), esquerda(NULL), direita(NULL) {}
};

// Inicializa as conex�es entre as cidades com valor infinito
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

// Adiciona cidade e seus vizinhos com as suas dist�ncias
void adicionarCidade(int codigo, const char *nome, bool centroPokemon, int vizinhos[], int distancias[], int numVizinhos)
{
    if (codigo >= MAX_CIDADES)
    {
        cout << "C�digo de cidade excede o m�ximo permitido.\n";
        return;
    }
    strcpy(cidades[codigo].nome, nome);
    cidades[codigo].possuiCentroPokemon = centroPokemon;

    // Define as conex�es no grafo com as dist�ncias fornecidas
    for (int i = 0; i < numVizinhos; ++i)
    {
        int vizinho = vizinhos[i];
        int distancia = distancias[i];
        if (vizinho < MAX_CIDADES)
        {
            cidades[codigo].conexoes[vizinho] = distancia; // Define a dist�ncia
            cidades[vizinho].conexoes[codigo] = distancia; // Grafo n�o direcionado
        }
    }
    numCidades = max(numCidades, codigo + 1);
}
// Usa Dijkstra para encontrar a rota mais pr�xima at� um centro Pok�mon
void encontrarCentroPokemonMaisProximo(int cidadeAtual)
{
    int dist[MAX_CIDADES];
    bool visitado[MAX_CIDADES];
    int predecessores[MAX_CIDADES];

    // Inicializa as dist�ncias e o array de visitados
    for (int i = 0; i < MAX_CIDADES; ++i)
    {
        dist[i] = INF;
        visitado[i] = false;
        predecessores[i] = -1;
    }
    dist[cidadeAtual] = 0;

    for (int i = 0; i < numCidades; ++i)
    {
        // Encontra o v�rtice com a menor dist�ncia que ainda n�o foi visitado
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
            break; // N�o h� mais v�rtices a serem visitados
        visitado[u] = true;

        // Atualiza as dist�ncias para os vizinhos de 'u'
        for (int v = 0; v < numCidades; ++v)
        {
            if (!visitado[v] && cidades[u].conexoes[v] != INF && dist[u] + cidades[u].conexoes[v] < dist[v])
            {
                dist[v] = dist[u] + cidades[u].conexoes[v];
                predecessores[v] = u;
            }
        }
    }

    // Encontra o centro Pok�mon mais pr�ximo
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
        cout << "Nenhum centro Pok�mon acess�vel a partir da cidade atual.\n";
        return;
    }

    // Exibe a rota at� o centro Pok�mon mais pr�ximo
    cout << "Rota para o centro Pok�mon mais pr�ximo (" << cidades[centroMaisProximo].nome << "):\n";
    int caminho[MAX_CIDADES];
    int tamanhoCaminho = 0;
    for (int v = centroMaisProximo; v != -1; v = predecessores[v])
    {
        caminho[tamanhoCaminho++] = v;
    }

    // Exibe o caminho na ordem correta
    for (int i = tamanhoCaminho - 1; i >= 0; --i)
    {
        cout << cidades[caminho[i]].nome;
        if (i > 0)
            cout << " -> ";
    }
    cout << "\nDist�ncia total: " << menorDistancia << "\n";
}

// Função para inserir um novo Pokémon na árvore binária
Node *inserir(Node *raiz, Pokemon p)
{
    if (raiz == NULL)
    {
        return new Node(p);
    }
    if (p.nome < raiz->pokemon.nome)
    {
        raiz->esquerda = inserir(raiz->esquerda, p);
    }
    else if (p.nome > raiz->pokemon.nome)
    {
        raiz->direita = inserir(raiz->direita, p);
    }
    return raiz;
}

// Função para buscar um Pokémon pelo nome na árvore binária
bool buscar(Node *raiz, string nome)
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
        return buscar(raiz->esquerda, nome);
    }
    else
    {
        return buscar(raiz->direita, nome);
    }
}

// Função para imprimir as informações dos Pokémons em ordem crescente dos nomes
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

// Função para coletar Pokémons em um array
void coletarPokemons(Node *raiz, Pokemon pokemons[], int &index)
{
    if (raiz != NULL)
    {
        coletarPokemons(raiz->esquerda, pokemons, index);
        pokemons[index++] = raiz->pokemon;
        coletarPokemons(raiz->direita, pokemons, index);
    }
}

// Função para ordenar os Pokémons por tipo (usando bubble sort)
void ordenarPorTipo(Pokemon pokemons[], int n)
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

// Função para imprimir os Pokémons por tipo
void imprimirPorTipo(Node *raiz)
{
    if (raiz == NULL)
        return;

    const int MAX_POKEMONS = 100; // Define a capacidade máxima para simplificação
    Pokemon pokemons[MAX_POKEMONS];
    int index = 0;

    coletarPokemons(raiz, pokemons, index);
    ordenarPorTipo(pokemons, index);

    for (int i = 0; i < index; ++i)
    {
        cout << "Nome: " << pokemons[i].nome
             << " - Tipo: " << pokemons[i].tipo
             << " - Numero: " << pokemons[i].numero
             << " - Localizacao: (" << pokemons[i].x << ", " << pokemons[i].y << ")\n";
    }
}

// Função para contar quantos Pokémons têm determinado tipo
int contarPorTipo(Node *raiz, string tipo)
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
    return count + contarPorTipo(raiz->esquerda, tipo) + contarPorTipo(raiz->direita, tipo);
}

// Função para desalocar a memória da árvore binária
void destruirArvore(Node *raiz)
{
    if (raiz != NULL)
    {
        destruirArvore(raiz->esquerda);
        destruirArvore(raiz->direita);
        delete raiz;
    }
}

int main()
{
    setlocale(LC_ALL, "pt_BR.UTF-8");
    Node *raiz = NULL;
    int opcao = -1;

    // Inicializa o grafo
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

                        cout << "\nDigite o c�digo da cidade: ";
                        cin >> codigo;
                        cin.ignore();
                        cout << "Digite o nome da cidade: ";
                        cin.getline(nome, 30);
                        cout << "A cidade possui centro Pok�mon? (Digite 1 para SIM ou Digite 0 para N�O): ";
                        cin >> centroPokemon;

                        cout << "Quantas cidades vizinhas? ";
                        cin >> numVizinhos;
                        int vizinhos[MAX_CIDADES];
                        int distancias[MAX_CIDADES];
                        cout << endl;

                        cout << "Digite os c�digos das cidades vizinhas e as dist�ncias:\n";
                        for (int j = 0; j < numVizinhos; ++j)
                        {
                            cout << "C�digo da cidade vizinha: ";
                            cin >> vizinhos[j];
                            cout << "Dist�ncia at� essa cidade: ";
                            cin >> distancias[j];
                        }

                        adicionarCidade(codigo, nome, centroPokemon, vizinhos, distancias, numVizinhos);
                    }
                    break;
                }
                case 2:
                {
                    cout << "\nDigite o c�digo da cidade atual para buscar o centro Pok�mon mais pr�ximo: ";
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
                cout << "1. Inserir Pokemon\n2. Imprimir Pokemons em ordem alfabetica\n3. Verificar Pokemon por nome\n4. Imprimir Pokemons por tipo\n5. Contar Pokemons por tipo\n0. Sair\n";
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
                    cout << "Localizacao (x y): ";
                    cin >> x >> y;

                    Pokemon p(nome, tipo, numero, x, y);
                    raiz = inserir(raiz, p);
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

                    if (buscar(raiz, nome))
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
                    imprimirPorTipo(raiz);
                }
                else if (op == 5)
                {
                    string tipo;
                    cout << "Tipo do Pokemon: ";
                    cin >> tipo;
                    int count = contarPorTipo(raiz, tipo);
                    cout << "Numero de Pokemons do tipo " << tipo << ": " << count << "\n";
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
