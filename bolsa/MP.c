#include "mp.h"

#define mpName TEXT("SharedMemory")


MP atualizarBoard(Empresa empresas[], DWORD numEmpresas, UltimaTransacao ultimaTransacao) {

    MP mp;

    // Abrir a memória partilhada
    mp.hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        mpName
    );

    if (mp.hMapFile == NULL || mp.hMapFile == 0) {
        Abort(_T("Não foi possível abrir o objecto de mapeamento de memória partilhada.\n"));
    }

    mp.pBuf = (SharedData*)MapViewOfFile(mp.hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        0);

    if (mp.pBuf == NULL) {
        CloseHandle(mp.hMapFile);
        Abort(_T("Não foi possível mapear o ficheiro.\n"));
    }

    for (DWORD i = 0; i < numEmpresas; i++) {

        _tcscpy_s(mp.pBuf->empresas[i].nome, STR_LEN, empresas[i].nome);
        mp.pBuf->empresas[i].num_acoes = empresas[i].num_acoes;
        mp.pBuf->empresas[i].preco_acao = empresas[i].preco_acao;

    }

    _tcscpy_s(mp.pBuf->ultimaTransacao.nome, STR_LEN, ultimaTransacao.nome);
    mp.pBuf->ultimaTransacao.num_acoes = ultimaTransacao.num_acoes;
    mp.pBuf->ultimaTransacao.preco_acao = ultimaTransacao.preco_acao;

    MensagemInfo(_T("Atualização do board bem sucedida."));

    return mp;
}

