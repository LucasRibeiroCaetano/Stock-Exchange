#include "eventos.h"
#include "utils.h"

// Função para criar os eventos
Eventos CriarEventos() {
    Eventos eventos;

    // Criação do evento de atualização
    eventos.hUpdate = CreateEvent(NULL, TRUE, TRUE, _T("NeedUpdate"));
    if (eventos.hUpdate == NULL) {
        Abort(_T("Erro na Criação do evento de atualização.\n"));
    }

    // Criação do evento de leitura
    eventos.hRead = CreateEvent(NULL, TRUE, FALSE, _T("CanRead"));
    if (eventos.hRead == NULL) {
        CloseHandle(eventos.hUpdate);
        Abort(_T("Erro na Criação do evento de leitura.\n"));
    }

    MensagemInfo(_T("Eventos criados com sucesso.\n\n"));

    return eventos;
}

// Função para alternar o estado do evento de atualização
void AlternarEventoAtualizacao(HANDLE hUpdate) {

    // Alternar o estado do evento nomeado
    if (WaitForSingleObject(hUpdate, 0) == WAIT_TIMEOUT) {
        // O evento está atualmente não assinalado, então definir para assinalado
        SetEvent(hUpdate);
        MensagemInfo(_T("Evento NeedUpdate ligado.\n"));
    }
    else {
        // O evento está atualmente assinalado, então redefinir para não assinalado
        ResetEvent(hUpdate);
        MensagemInfo(_T("Evento NeedUpdate desligado.\n"));
    }
}

// Função para alternar o estado do evento de leitura
void AlternarEventoLeitura(HANDLE hRead) {

    // Alternar o estado do evento nomeado
    if (WaitForSingleObject(hRead, 0) == WAIT_TIMEOUT) {
        // O evento está atualmente não assinalado, então definir para assinalado
        SetEvent(hRead);
        MensagemInfo(_T("Evento CanRead ligado.\n"));
    }
    else {
        // O evento está atualmente assinalado, então redefinir para não assinalado
        ResetEvent(hRead);
        MensagemInfo(_T("Evento CanRead desligado.\n"));
    }
}