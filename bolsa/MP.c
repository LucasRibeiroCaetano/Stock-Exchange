#include "data.h"
#include "utils.h"

#define MP TEXT("SharedMemory")


void atualizarBoard(Empresa empresas[], DWORD numEmpresas, UltimaTransacao ultimaTransacao) {

    HANDLE hMapFile;
    SharedData* pBuf;

    // Abrir a mem�ria partilhada
    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        MP
    );

    if (hMapFile == NULL) {
        Abort(_T("N�o foi poss�vel abrir o objecto de mapeamento de mem�ria partilhada.\n"));
    }

    pBuf = (SharedData*)MapViewOfFile(hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        0);

    if (pBuf == NULL) {
        CloseHandle(hMapFile);
        Abort(_T("N�o foi poss�vel mapear o ficheiro.\n"));
    }



    for (DWORD i = 0; i < numEmpresas; i++) {

        _tcscpy_s(pBuf->empresas[i].nome, STR_LEN, empresas[i].nome);
        pBuf->empresas[i].num_acoes = empresas[i].num_acoes;
        pBuf->empresas[i].preco_acao = empresas[i].preco_acao;

    }

    _tcscpy_s(pBuf->ultimaTransacao.nome, STR_LEN, ultimaTransacao.nome);
    pBuf->ultimaTransacao.num_acoes = ultimaTransacao.num_acoes;
    pBuf->ultimaTransacao.preco_acao = ultimaTransacao.preco_acao;

    MensagemInfo(_T("Atualiza��o do board bem sucedida.\n\n"));


    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
}

