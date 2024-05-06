#define UNICODE

#include <fcntl.h>
#include <io.h>

#include "data.h"

int _tmain(int argc, TCHAR* argv[]) {

    // Set Mode Verification
    DWORD stdinReturn;
    DWORD stdoutReturn;
    DWORD stderrReturn;

    // Verificação do Set Mode
    if (stdinReturn == -1 || stdoutReturn == -1 || stderrReturn == -1)
        Abort(_T("Erro no _setmode."));

    if (argc != 2) {
        Abort(_T("Sintaxe Errada -> board <número de empresas>\n"));
    }

#ifdef UNICODE 
    stdinReturn = _setmode(_fileno(stdin), _O_WTEXT);
    stdoutReturn = _setmode(_fileno(stdout), _O_WTEXT);
    stderrReturn = _setmode(_fileno(stderr), _O_WTEXT);
#endif


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
        printf("Could not open file mapping object (%d).\n", GetLastError());
        return 1;
    }

    // Map the shared memory into the process address space
    pBuf = (SharedData*)MapViewOfFile(hMapFile,   // Handle to map object
        FILE_MAP_ALL_ACCESS, // Read/write permission
        0,
        0,
        0);

    if (pBuf == NULL) {
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    // Print data from the shared memory
    printf("Empresas:\n");
    for (int i = 0; i < MAX_EMPRESAS; i++) {
        printf("Nome: %s, Número de ações: %d, Preço da ação: %.2f\n",
            pBuf->empresas[i].nome,
            pBuf->empresas[i].num_acoes,
            pBuf->empresas[i].preco_acao);
    }

    printf("\nUtilizadores:\n");
    for (int i = 0; i < pBuf->numUtilizadores; i++) {
        printf("Username: %s, Saldo: %.2f, Online: %d\n",
            pBuf->utilizadores[i].username,
            pBuf->utilizadores[i].saldo,
            pBuf->utilizadores[i].online);
    }

    printf("\nÚltima Transação:\n");
    printf("Nome: %s, Número de ações: %d, Preço da ação: %.2f\n",
        pBuf->ultimaTransacao.nome,
        pBuf->ultimaTransacao.num_acoes,
        pBuf->ultimaTransacao.preco_acao);

    // Unmap the shared memory
    UnmapViewOfFile(pBuf);

    // Close the handle to the shared memory
    CloseHandle(hMapFile);





































    //----------------------------------------------- MP -----------------------------------------------






}