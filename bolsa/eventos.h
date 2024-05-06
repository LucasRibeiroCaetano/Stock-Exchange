#pragma once

#include <windows.h>
#include <tchar.h>

// Defina identificadores �nicos para os eventos
#define UPDATE_EVENT_NAME _T("NeedUpdate")
#define READ_EVENT_NAME _T("CanRead")


void CriarEventos();
void AlternarEventoAtualizacao();
void AlternarEventoLeitura();