#include <tchar.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define N_EMPRESAS 5 // M�ximo de a��es em empresas diferentes que um utilizador pode ter (ex: Apple, Google, Microsoft, SpaceX, Facebook)
#define MAX_EMPRESAS 30 // M�ximo de empresas que pode haver no total
#define STR_LEN 50
#define MAX_USERS 20
#define MAX_PARAM 5
#define SHARED_MEM_SIZE sizeof(SharedData)

// Empresa
typedef struct {
    TCHAR nome[STR_LEN];
    int num_acoes;
    float preco_acao;
} Empresa;

// Carteira de a��es de um utilizador
typedef struct {
    TCHAR username[STR_LEN];
    Empresa empresas[N_EMPRESAS];
} CarteiraAcoes;

// Utilizador
typedef struct {
    TCHAR username[STR_LEN];
    TCHAR password[STR_LEN];
    float saldo;
    bool online;
} Utilizador;

// �ltima Transa��o
typedef struct {
    TCHAR nome[STR_LEN]; // Nome da empresa
    int num_acoes; // N�mero de a��es compradas
    float preco_acao; // Pre�o ao qual as a��es foram compradas
} UltimaTransacao;

// Mem�ria Partilhada
typedef struct {
    DWORD numEmpresas;
    Empresa empresas[MAX_EMPRESAS];
    UltimaTransacao ultimaTransacao;
} SharedData;

// Dados da MP
typedef struct {
    HANDLE hMapFile;
    SharedData* pBuf;
} MP;

// Handles dos eventos
typedef struct {
    HANDLE hUpdate;
    HANDLE hRead;
} Eventos;
