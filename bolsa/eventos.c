#include "eventos.h"
#include "utils.h"

// Função para criar os eventos
Eventos CriarEventos() {
    Eventos eventos;

    // Criação do evento de leitura
    eventos.hRead = CreateEvent(NULL, TRUE, FALSE, _T("CanRead"));
    if (eventos.hRead == NULL) {
        Abort(_T("Erro na Criação do evento de leitura.\n"));
    }

    MensagemInfo(_T("Eventos criados com sucesso.\n\n"));

    return eventos;
}