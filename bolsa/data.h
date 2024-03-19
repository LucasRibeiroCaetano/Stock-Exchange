#include <tchar.h>

#define MAX_EMPRESAS 5
#define STR_LEN 50
#define MAX_USERS 20 // No máximo podemos ter 20 utilizadores

// Empresa
typedef struct {
    TCHAR nome[STR_LEN];
    int num_acoes;
    float preco_acao;
} Empresa;

// Carteira de ações de um utilizador
typedef struct {
    TCHAR username[STR_LEN];
    Empresa empresas[MAX_EMPRESAS];
} CarteiraAcoes;

// Utilizador
typedef struct {
    TCHAR username[STR_LEN];
    TCHAR password[STR_LEN];
    float saldo;
} Utilizador;