#define UNICODE

#include <fcntl.h>
#include <io.h>

#include "data.h"

// Fun��o de compara��o para qsort
int compare(const void* a, const void* b) {
    const Empresa* empresaA = (const Empresa*)a;
    const Empresa* empresaB = (const Empresa*)b;
    if (empresaA->preco_acao < empresaB->preco_acao)
        return 1;
    else if (empresaA->preco_acao > empresaB->preco_acao)
        return -1;
    else
        return 0;
}

int _tmain(int argc, TCHAR* argv[]) {

    // Argumento
    DWORD N;

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

    if (_stscanf_s(argv[1], _T("%lu"), &N) != 1) {
        _tprintf_s(_T("Erro ao converter o n�mero.\n"));
        return 1;
    }



    if (N > 10 || N < 1)
        Abort(_T("O par�metro N tem de estar entre 1 e 10.\n"));

    // Limpar a consola
    limparConsola();

    _tprintf_s(_T("\n*********************************************************\n"));
    _tprintf_s(_T("*                                                       *\n"));
    _tprintf_s(_T("*                         Board                         *\n"));
    _tprintf_s(_T("*                                                       *\n"));
    _tprintf_s(_T("*********************************************************\n"));


    MP mp;
    Eventos eventos;
    DWORD result;

    // Abre o evento de leitura para ver se j� podemos ler
    eventos.hRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("CanRead"));
    if (eventos.hRead == NULL) {
        Abort("Erro ao abrir o evento.\n");
    }

    // Abre o evento de atualiza��o para informar o servidor de quando pode atualizar
    eventos.hUpdate = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("NeedUpdate"));
    if (eventos.hUpdate == NULL) {
        Abort("Erro ao abrir o evento.\n");
    }

    while (true) {

        MensagemInfo(_T("� espera para atualizar a informa��o..."));
        // Wait for the event to be signaled
        result = WaitForSingleObject(eventos.hRead, INFINITE);

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
        else if (result == WAIT_ABANDONED) {
            Abort(_T("[WAIT_ABANDONED]: Erro ao esperar pelo evento.\n"));
        }
        else if (result == WAIT_TIMEOUT) {
            Abort(_T("[WAIT_TIMEOUT]: Erro ao esperar pelo evento.\n"));
        }
        else {
            Abort(_T("Erro ao esperar pelo evento.\n"));
        }

        if (N > mp.pBuf->numEmpresas)
            N = mp.pBuf->numEmpresas;

        // Isto tem de ser feito infinitamente at� a 

        // Limpar a consola
        limparConsola();

        _tprintf_s(_T("\n*********************************************************\n"));
        _tprintf_s(_T("*                                                       *\n"));
        _tprintf_s(_T("*                         Board                         *\n"));
        _tprintf_s(_T("*                                                       *\n"));
        _tprintf_s(_T("*********************************************************\n"));

        _tprintf_s(_T("\nTop %lu Empresas\n\n"), N);

        // Ordena as empresas pelo valor das a��es
        qsort(mp.pBuf->empresas, sizeof(mp.pBuf->empresas) / sizeof(mp.pBuf->empresas[0]), sizeof(mp.pBuf->empresas[0]), compare);

        for (DWORD i = 0; i < N; i++) {
            _tprintf_s(_T("%d. %s - %.2f\n"), i + 1, mp.pBuf->empresas[i].nome, mp.pBuf->empresas[i].preco_acao);
        }

        _tprintf_s(_T("\n�ltima Transa��o:\n"));
        _tprintf_s(_T("Nome: %s, N�mero de a��es: %d, Pre�o da a��o: %.2f\n\n"),
            mp.pBuf->ultimaTransacao.nome,
            mp.pBuf->ultimaTransacao.num_acoes,
            mp.pBuf->ultimaTransacao.preco_acao);
        
        // Informa que o servidor j� pode enviar informa��o atualizada
        SetEvent(eventos.hUpdate);

        MensagemInfo(_T("Evento de atualiza��o ligado."));

        // Vamos esperar at� poder ler outra vez
        ResetEvent(eventos.hRead);

        MensagemInfo(_T("Evento de leitura desligado."));
    }

    return 0;
}

