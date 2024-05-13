#include "eventos.h"
#include "utils.h"

// Fun��o para criar os eventos
Eventos CriarEventos() {
    Eventos eventos;

    // Cria��o do evento de leitura
    eventos.hRead = CreateEvent(NULL, TRUE, FALSE, _T("CanRead"));
    if (eventos.hRead == NULL) {
        Abort(_T("Erro na Cria��o do evento de leitura.\n"));
    }

    MensagemInfo(_T("Eventos criados com sucesso.\n\n"));

    return eventos;
}