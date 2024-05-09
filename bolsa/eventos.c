#include "eventos.h"
#include "utils.h"

// Fun��o para criar os eventos
Eventos CriarEventos() {
    Eventos eventos;

    // Cria��o do evento de atualiza��o
    eventos.hUpdate = CreateEvent(NULL, TRUE, TRUE, _T("NeedUpdate"));
    if (eventos.hUpdate == NULL) {
        Abort(_T("Erro na Cria��o do evento de atualiza��o.\n"));
    }

    // Cria��o do evento de leitura
    eventos.hRead = CreateEvent(NULL, TRUE, FALSE, _T("CanRead"));
    if (eventos.hRead == NULL) {
        CloseHandle(eventos.hUpdate);
        Abort(_T("Erro na Cria��o do evento de leitura.\n"));
    }

    MensagemInfo(_T("Eventos criados com sucesso.\n\n"));

    return eventos;
}

// Fun��o para alternar o estado do evento de atualiza��o
void AlternarEventoAtualizacao(HANDLE hUpdate) {

    // Alternar o estado do evento nomeado
    if (WaitForSingleObject(hUpdate, 0) == WAIT_TIMEOUT) {
        // O evento est� atualmente n�o assinalado, ent�o definir para assinalado
        SetEvent(hUpdate);
        MensagemInfo(_T("Evento NeedUpdate ligado.\n"));
    }
    else {
        // O evento est� atualmente assinalado, ent�o redefinir para n�o assinalado
        ResetEvent(hUpdate);
        MensagemInfo(_T("Evento NeedUpdate desligado.\n"));
    }
}

// Fun��o para alternar o estado do evento de leitura
void AlternarEventoLeitura(HANDLE hRead) {

    // Alternar o estado do evento nomeado
    if (WaitForSingleObject(hRead, 0) == WAIT_TIMEOUT) {
        // O evento est� atualmente n�o assinalado, ent�o definir para assinalado
        SetEvent(hRead);
        MensagemInfo(_T("Evento CanRead ligado.\n"));
    }
    else {
        // O evento est� atualmente assinalado, ent�o redefinir para n�o assinalado
        ResetEvent(hRead);
        MensagemInfo(_T("Evento CanRead desligado.\n"));
    }
}