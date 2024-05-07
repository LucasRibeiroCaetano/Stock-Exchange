#include "eventos.h"
#include "utils.h"

// Fun��o para criar os eventos
void CriarEventos() {

    HANDLE hUpdateEvent = NULL;
    HANDLE hReadEvent = NULL;

    // Cria��o do evento de atualiza��o
    hUpdateEvent = CreateEvent(NULL, TRUE, FALSE, _T("NeedUpdate"));
    if (hUpdateEvent == NULL) {
        Abort(_T("Erro na Cria��o do evento de atualiza��o.\n"));
    }

    // Cria��o do evento de leitura
    hReadEvent = CreateEvent(NULL, TRUE, FALSE, _T("CanRead"));
    if (hReadEvent == NULL) {
        CloseHandle(hUpdateEvent);
        Abort(_T("Erro na Cria��o do evento de leitura.\n"));
    }

    MensagemInfo(_T("Eventos criados com sucesso.\n\n"));

    // Fechar os eventos, pois n�o ser�o usados diretamente nesta fun��o
    CloseHandle(hUpdateEvent);
    CloseHandle(hReadEvent);
}

// Fun��o para alternar o estado do evento de atualiza��o
void AlternarEventoAtualizacao() {
    // Abrir o evento nomeado
    HANDLE hEventoAtualizacao = OpenEvent(EVENT_MODIFY_STATE, FALSE, _T("NeedUpdate"));
    if (hEventoAtualizacao == NULL) {
        Abort(_T("Erro ao abrir o evento de atualiza��o.\n"));
    }

    // Alternar o estado do evento nomeado
    if (WaitForSingleObject(hEventoAtualizacao, 0) == WAIT_TIMEOUT) {
        // O evento est� atualmente n�o assinalado, ent�o definir para assinalado
        SetEvent(hEventoAtualizacao);
        MensagemInfo(_T("Evento NeedUpdate ligado.\n"));
    }
    else {
        // O evento est� atualmente assinalado, ent�o redefinir para n�o assinalado
        ResetEvent(hEventoAtualizacao);
        MensagemInfo(_T("Evento NeedUpdate desligado.\n"));
    }

    // Fechar a handle do evento nomeado
    CloseHandle(hEventoAtualizacao);
}

// Fun��o para alternar o estado do evento de leitura
void AlternarEventoLeitura() {
    // Abrir o evento nomeado
    HANDLE hEventoLeitura = OpenEvent(EVENT_MODIFY_STATE, FALSE, _T("CanRead"));
    if (hEventoLeitura == NULL) {
        // Falha ao abrir o evento nomeado
        Abort(_T("Erro ao abrir o evento de leitura.\n"));
    }

    // Alternar o estado do evento nomeado
    if (WaitForSingleObject(hEventoLeitura, 0) == WAIT_TIMEOUT) {
        // O evento est� atualmente n�o assinalado, ent�o definir para assinalado
        SetEvent(hEventoLeitura);
        MensagemInfo(_T("Evento CanRead ligado.\n"));
    }
    else {
        // O evento est� atualmente assinalado, ent�o redefinir para n�o assinalado
        ResetEvent(hEventoLeitura);
        MensagemInfo(_T("Evento CanRead desligado.\n"));
    }

    // Fechar a handle do evento nomeado
    CloseHandle(hEventoLeitura);
}