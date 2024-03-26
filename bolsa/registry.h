#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define KEY_PATH _T("Software\\SO2")
#define KEY_NAME _T("NCLIENTES")

int chaveExiste();
void criarChave();
DWORD obterValor();

