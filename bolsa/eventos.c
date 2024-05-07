#include "eventos.h"
#include "utils.h"

// Função para criar os eventos
void CriarEventos() {

    HANDLE hUpdateEvent = NULL;
    HANDLE hReadEvent = NULL;

    // Criação do evento de atualização
    hUpdateEvent = CreateEvent(NULL, TRUE, FALSE, _T("NeedUpdate"));
    if (hUpdateEvent == NULL) {
        Abort(_T("Erro na Criação do evento de atualização.\n"));
    }

    // Criação do evento de leitura
    hReadEvent = CreateEvent(NULL, TRUE, FALSE, _T("CanRead"));
    if (hReadEvent == NULL) {
        CloseHandle(hUpdateEvent);
        Abort(_T("Erro na Criação do evento de leitura.\n"));
    }

    MensagemInfo(_T("Eventos criados com sucesso.\n\n"));

    // Fechar os eventos, pois não serão usados diretamente nesta função
    CloseHandle(hUpdateEvent);
    CloseHandle(hReadEvent);
}

// Função para alternar o estado do evento de atualização
void AlternarEventoAtualizacao() {
    // Abrir o evento nomeado
    HANDLE hEventoAtualizacao = OpenEvent(EVENT_MODIFY_STATE, FALSE, _T("NeedUpdate"));
    if (hEventoAtualizacao == NULL) {
        Abort(_T("Erro ao abrir o evento de atualização.\n"));
    }

    // Alternar o estado do evento nomeado
    if (WaitForSingleObject(hEventoAtualizacao, 0) == WAIT_TIMEOUT) {
        // O evento está atualmente não assinalado, então definir para assinalado
        SetEvent(hEventoAtualizacao);
        MensagemInfo(_T("Evento NeedUpdate ligado.\n"));
    }
    else {
        // O evento está atualmente assinalado, então redefinir para não assinalado
        ResetEvent(hEventoAtualizacao);
        MensagemInfo(_T("Evento NeedUpdate desligado.\n"));
    }

    // Fechar a handle do evento nomeado
    CloseHandle(hEventoAtualizacao);
}

// Função para alternar o estado do evento de leitura
void AlternarEventoLeitura() {
    // Abrir o evento nomeado
    HANDLE hEventoLeitura = OpenEvent(EVENT_MODIFY_STATE, FALSE, _T("CanRead"));
    if (hEventoLeitura == NULL) {
        // Falha ao abrir o evento nomeado
        Abort(_T("Erro ao abrir o evento de leitura.\n"));
    }

    // Alternar o estado do evento nomeado
    if (WaitForSingleObject(hEventoLeitura, 0) == WAIT_TIMEOUT) {
        // O evento está atualmente não assinalado, então definir para assinalado
        SetEvent(hEventoLeitura);
        MensagemInfo(_T("Evento CanRead ligado.\n"));
    }
    else {
        // O evento está atualmente assinalado, então redefinir para não assinalado
        ResetEvent(hEventoLeitura);
        MensagemInfo(_T("Evento CanRead desligado.\n"));
    }

    // Fechar a handle do evento nomeado
    CloseHandle(hEventoLeitura);
}