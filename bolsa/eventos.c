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