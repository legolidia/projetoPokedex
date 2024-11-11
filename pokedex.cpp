#include <iostream>
#include <cstring>
#include <climits>
#include <locale>

using namespace std;

const int MAX_CIDADES = 100; // Define o tamanho m�ximo de cidades no grafo
const int INF = INT_MAX;     // Conex�o inexistente ou caminho imposs�vel de alcan�ar no grafo

// Armazenar dados de cada cidade
struct Cidade
{
    char nome[30];
    bool possuiCentroPokemon;
    int conexoes[MAX_CIDADES]; // Armazena as dist�ncias para outras cidades
};

Cidade cidades[MAX_CIDADES]; // Array de cidades
int numCidades = 0;

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

int main()
{
    setlocale(LC_ALL, "pt_BR.UTF-8");

    // Inicializa o grafo
    inicializarGrafo();

    int op;
    cout << "---------------POK�DEX----------------" << endl;
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

    return 0;
}
