#pragma once

#include "data.h"
#include "utils.h"

// Dados da MP
typedef struct {
    HANDLE hMapFile;
    SharedData* pBuf;
} MP;


void atualizarBoard(MP mp, Empresa empresas[], DWORD numEmpresas, UltimaTransacao ultimaTransacao);