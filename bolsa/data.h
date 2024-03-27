#include <tchar.h>
#include <stdbool.h>

#define N_EMPRESAS 5 // Máximo de ações em empresas diferentes que um utilizador pode ter (ex: Apple, Google, Microsoft, SpaceX, Facebook)
#define MAX_EMPRESAS 30 // Máximo de empresas que pode haver no total
#define STR_LEN 50
#define MAX_USERS 20
#define MAX_PARAM 5

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
} CarteiraAcoes;

// Utilizador
typedef struct {
    TCHAR username[STR_LEN];
    TCHAR password[STR_LEN];
    float saldo;
    bool online;
} Utilizador;