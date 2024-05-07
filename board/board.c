#define UNICODE

#include <fcntl.h>
#include <io.h>

#include "data.h"

int _tmain(int argc, TCHAR* argv[]) {

    // Set Mode Verification
    DWORD stdinReturn = 0;
    DWORD stdoutReturn = 0;
    DWORD stderrReturn = 0;

#ifdef UNICODE
    stdinReturn = _setmode(_fileno(stdin), _O_WTEXT);
    stdoutReturn = _setmode(_fileno(stdout), _O_WTEXT);
    stderrReturn = _setmode(_fileno(stderr), _O_WTEXT);
#endif

    // Verificação do Set Mode
    if (stdinReturn == -1 || stdoutReturn == -1 || stderrReturn == -1)
        Abort(_T("Erro no _setmode."));

    if (argc != 2) {
        Abort(_T("Sintaxe Errada -> board <número de empresas>\n"));
    }


    //----------------------------------------------- MP -----------------------------------------------
    HANDLE hMapFile;
    SharedData* pBuf;

    // Open the shared memory
    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS, // Read/write permission
        FALSE,               // Do not inherit the name
        TEXT("SharedMemory") // Name of the mapping object
    );

    if (hMapFile == NULL) {
        Abort(_T("Não foi possível abrir o objecto de mapeamento de memória partilhada.\n"));
        return 1;
    }

    // Map the shared memory into the process address space
    pBuf = (SharedData*)MapViewOfFile(hMapFile,   // Handle to map object
        FILE_MAP_ALL_ACCESS, // Read/write permission
        0,
        0,
        0);

    if (pBuf == NULL) {
        Abort(_T("Não foi possível mapear o ficheiro.\n"));
        CloseHandle(hMapFile);
        return 1;
    }

    // Print data from the shared memory
    _tprintf_s(_T("nEmpresas: %d\n"), pBuf->numEmpresas);
    _tprintf_s(_T("Empresas:\n"));
    for (int i = 0; i < pBuf->numEmpresas; i++) {
        _tprintf_s(_T("Nome: %s, Número de ações: %d, Preço da ação: %.2f\n"),
            pBuf->empresas[i].nome,
            pBuf->empresas[i].num_acoes,
            pBuf->empresas[i].preco_acao);
    }

    _tprintf_s(_T("\nÚltima Transação:\n"));
    _tprintf_s(_T("Nome: %s, Número de ações: %d, Preço da ação: %.2f\n"),
        pBuf->ultimaTransacao.nome,
        pBuf->ultimaTransacao.num_acoes,
        pBuf->ultimaTransacao.preco_acao);


    // Acabei de Ler, libertar os recursos da memória partilhada
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);



































    //----------------------------------------------- MP -----------------------------------------------



    return 0;
}