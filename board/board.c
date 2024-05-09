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

    // Verifica��o do Set Mode
    if (stdinReturn == -1 || stdoutReturn == -1 || stderrReturn == -1)
        Abort(_T("Erro no _setmode."));

    if (argc != 2) {
        Abort(_T("Sintaxe Errada -> board <n�mero de empresas>\n"));
    }


    //----------------------------------------------- MP -----------------------------------------------
    MP mp;
    Eventos eventos;

    // Abre o evento de leitura para ver se j� podemos ler
    eventos.hRead = OpenEvent(EVENT_MODIFY_STATE, FALSE, _T("CanRead"));
    if (eventos.hRead == NULL) {
        Abort("Erro ao abrir o evento.\n");
    }

    // Wait for the event to be signaled
    DWORD result = WaitForSingleObject(eventos.hRead, INFINITE);
    if (result == WAIT_OBJECT_0) {

        // Podemos ler
        mp.hMapFile = OpenFileMapping(
            FILE_MAP_ALL_ACCESS, // Read/write permission
            FALSE,               // Do not inherit the name
            TEXT("SharedMemory") // Name of the mapping object
        );

        if (mp.hMapFile == NULL) {
            Abort(_T("N�o foi poss�vel abrir o objecto de mapeamento de mem�ria partilhada.\n"));
        }

        // Map the shared memory into the process address space
        mp.pBuf = (SharedData*)MapViewOfFile(mp.hMapFile,   // Handle to map object
            FILE_MAP_ALL_ACCESS, // Read/write permission
            0,
            0,
            0);

        if (mp.pBuf == NULL) {
            CloseHandle(mp.hMapFile);
            Abort(_T("N�o foi poss�vel mapear o ficheiro.\n"));
        }
    }
    else {
        Abort(_T("Erro ao esperar pelo evento.\n"));
    }

    //----------------------------------------------- MP -----------------------------------------------


    // Mostrar a informa��o lida
    _tprintf_s(_T("nEmpresas: %d\n"), mp.pBuf->numEmpresas);
    _tprintf_s(_T("Empresas:\n"));
    for (int i = 0; i < mp.pBuf->numEmpresas; i++) {
        _tprintf_s(_T("Nome: %s, N�mero de a��es: %d, Pre�o da a��o: %.2f\n"),
            mp.pBuf->empresas[i].nome,
            mp.pBuf->empresas[i].num_acoes,
            mp.pBuf->empresas[i].preco_acao);
    }

    _tprintf_s(_T("\n�ltima Transa��o:\n"));
    _tprintf_s(_T("Nome: %s, N�mero de a��es: %d, Pre�o da a��o: %.2f\n"),
        mp.pBuf->ultimaTransacao.nome,
        mp.pBuf->ultimaTransacao.num_acoes,
        mp.pBuf->ultimaTransacao.preco_acao);


    return 0;
}