#include <tchar.h>
#include <stdbool.h>
#include <windows.h>

#include "eventos.h"

#define N_EMPRESAS 5 // Máximo de ações em empresas diferentes que um utilizador pode ter (ex: Apple, Google, Microsoft, SpaceX, Facebook)
#define MAX_EMPRESAS 30 // Máximo de empresas que pode haver no total
#define STR_LEN 50
#define MAX_USERS 20
#define MAX_PARAM 5
#define SHARED_MEM_SIZE sizeof(SharedData)
#define PIPE_NAME _T("\\\\.\\pipe\\ClienteServidor")

struct MP;

// Empresa
typedef struct {
    TCHAR nome[STR_LEN];
    int num_acoes;
    float preco_acao;
} Empresa;

// Carteira de ações de um utilizador
typedef struct {
    TCHAR username[STR_LEN];
    Empresa empresas[N_EMPRESAS];
    DWORD numEmpresas;
} CarteiraAcoes;

// Utilizador
typedef struct {
    TCHAR username[STR_LEN];
    TCHAR password[STR_LEN];
    float saldo;
    bool online;
} Utilizador;

// Última Transação
typedef struct {
    TCHAR nome[STR_LEN]; // Nome da empresa
    int num_acoes; // Número de ações compradas
    float preco_acao; // Preço ao qual as ações foram compradas
} UltimaTransacao;

// Memória Partilhada
typedef struct {
    DWORD numEmpresas;
    Empresa empresas[MAX_EMPRESAS];
    UltimaTransacao ultimaTransacao;
} SharedData;

// Dados da MP
typedef struct MP {
    HANDLE hMapFile;
    SharedData* pBuf;
} MP;

// Estrutura para executar os comandos do cliente na thread
typedef struct {
    // O comando e os parâmetros
    TCHAR comando[STR_LEN];
    TCHAR params[2][STR_LEN];

    DWORD idPipe; // id do pipe no array hPipe

    // Dados
    TCHAR activeUser[STR_LEN]; // username do utilizador online
} DataClientes;

// Dados para a thread dos comandos do admin
typedef struct {
    HANDLE hPipes[MAX_USERS];
    HANDLE hThreads[MAX_USERS];
    Empresa* empresas;
    DWORD numEmpresas;
    UltimaTransacao ultimaTransacao;
    Eventos eventos;
    Utilizador* utilizadores;
    CarteiraAcoes* carteiras;
    DWORD numUtilizadores;
    struct MP mp;

    DataClientes dataClientes;
} DataAdmin;




void atualizarBoard(MP mp, Empresa empresas[], DWORD numEmpresas, UltimaTransacao ultimaTransacao);

