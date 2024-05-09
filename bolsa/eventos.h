#pragma once

#include <windows.h>
#include <tchar.h>

// Defina identificadores únicos para os eventos
#define UPDATE_EVENT_NAME _T("NeedUpdate")
#define READ_EVENT_NAME _T("CanRead")

// Handles dos eventos
typedef struct {
    HANDLE hUpdate;
    HANDLE hRead;
} Eventos;


Eventos CriarEventos();
void AlternarEventoAtualizacao(HANDLE hUpdate);
void AlternarEventoLeitura(HANDLE hRead);