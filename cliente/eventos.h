#pragma once

#include <windows.h>
#include <tchar.h>

#define READ_EVENT_NAME _T("CanRead")

// Handles dos eventos
typedef struct {
    HANDLE hRead;
} Eventos;


Eventos CriarEventos();