#include "mp.h"

#define mpName TEXT("SharedMemory")


void atualizarBoard(MP mp, Empresa empresas[], DWORD numEmpresas, UltimaTransacao ultimaTransacao) {

    mp.pBuf->numEmpresas = numEmpresas;

    for (DWORD i = 0; i < numEmpresas; i++) {

        _tcscpy_s(mp.pBuf->empresas[i].nome, STR_LEN, empresas[i].nome);
        mp.pBuf->empresas[i].num_acoes = empresas[i].num_acoes;
        mp.pBuf->empresas[i].preco_acao = empresas[i].preco_acao;

    }

    _tcscpy_s(mp.pBuf->ultimaTransacao.nome, STR_LEN, ultimaTransacao.nome);
    mp.pBuf->ultimaTransacao.num_acoes = ultimaTransacao.num_acoes;
    mp.pBuf->ultimaTransacao.preco_acao = ultimaTransacao.preco_acao;

    MensagemInfo(_T("Atualização do board bem sucedida.\n\n"));
}

