#define UNICODE

#include <io.h>
#include <fcntl.h>

#include "utils.h"

#define PIPE_NAME _T("\\\\.\\pipe\\ClienteServidor")
#define MAX_PARAM 2
#define STR_LEN 50

int main() {

    TCHAR comando[STR_LEN];
    TCHAR linhaAUX[STR_LEN];
    TCHAR linha[STR_LEN];
    DWORD nParam;
    DWORD dwWritten, dwRead;
    TCHAR param[MAX_PARAM][STR_LEN];
    TCHAR resultado[STR_LEN];
    BOOL valido;
    TCHAR activeUser[STR_LEN];
    DWORD dwWaitResult;


    // Para validar as operações que precisam de login
    DWORD loggedIn = false;

    DWORD stdinReturn;
    DWORD stdoutReturn;
    DWORD stderrReturn;

    HANDLE hPipe;
    HANDLE hClose;

#ifdef UNICODE 
    stdinReturn = _setmode(_fileno(stdin), _O_WTEXT);
    stdoutReturn = _setmode(_fileno(stdout), _O_WTEXT);
    stderrReturn = _setmode(_fileno(stderr), _O_WTEXT);
#endif

    // Verificação do Set Mode
    if (stdinReturn == -1 || stdoutReturn == -1 || stderrReturn == -1)
        Abort(_T("Erro no _setmode."));

    // Connect to the named pipe
    hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE) {
        MensagemInfo(_T("Número máximo de clientes atingido. Tente novamente mais tarde...\n"));
        return 1;
    }

    hClose = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("Close"));
    if (hClose == NULL) {
        Abort(_T("Erro ao abrir o evento close"));
    }

    limparConsola();

    // Interface de gestão de comandos
    _tprintf(_T("Escreva 'ajuda' para uma lista completa de comandos.\n"));

    // Ciclo de enviar comandos, copiar da bolsa
    while (1) {

        dwWaitResult = WaitForSingleObject(hClose, 0);

        if (dwWaitResult == WAIT_OBJECT_0) {

            MensagemInfo(_T("A Encerrar\n"));
            // Sair do ciclo e encerrar o programa
            return 0;
        }
        else if (dwWaitResult == WAIT_TIMEOUT); // Não foi sinalizado, não vamos fazer nada
        else {
            // Error occurred
            Erro(_T("\nErro ao esperar pelo evento hClose\n"));
        }

        _tprintf(_T("Cliente> "));

        dwWaitResult = WaitForSingleObject(hClose, 0);

        if (dwWaitResult == WAIT_OBJECT_0) {

            MensagemInfo(_T("A Encerrar\n"));
            // Sair do ciclo e encerrar o programa
            return 0;
        }
        else if (dwWaitResult == WAIT_TIMEOUT); // Não foi sinalizado, não vamos fazer nada
        else {
            // Error occurred
            Erro(_T("\nErro ao esperar pelo evento hClose\n"));
        }

        _fgetts(linhaAUX, STR_LEN, stdin);

        // Obtém o tamanho do comando
        size_t length = _tcslen(linhaAUX);

        // Se o comando não estiver vazio truncar o \n
        if (length > 0 && linhaAUX[length - 1] == '\n') {
            linhaAUX[length - 1] = '\0';
        }

        // linhaAUX irá ser alterado nas funções então copio o conteúdo para outra variável para guardar o input
        _tcscpy_s(linha, STR_LEN, linhaAUX);

        nParam = contaParametros(linhaAUX);

        // Repartir o comando e os parâmetros
        extrairParametros(nParam, linhaAUX, comando, param);

        if (!_tcsicmp(comando, _T("ajuda"))) {

            if (nParam == 0) {
                _tprintf_s(_T("\n*********************************************************\n\n"));
                _tprintf(_T("Acrescentar uma empresa\n - addc <nome-empresa> <n�mero-a��es> <preço-ação>\n\n"));
                _tprintf(_T("Ler as empresas de um ficheiro de texto\n - addf <nome-ficheiro>\n\n"));
                _tprintf(_T("Listar todas as empresas\n - listc\n\n"));
                _tprintf(_T("Redefinir custo das ações de uma empresa\n - stock <nome-empresa> <preço-ação>\n\n"));
                _tprintf(_T("Listar utilizadores\n - users\n\n"));
                _tprintf(_T("Pausar as operações de compra e venda\n - pause <n�mero-segundos>\n\n"));
                _tprintf(_T("Limpar a consola\n - limpar\n\n"));
                _tprintf(_T("Encerrar a plataforma\n - close\n\n"));
                _tprintf_s(_T("*********************************************************\n\n"));
            }
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));
        }

        else if (!_tcsicmp(comando, _T("login"))) {

            if (nParam == 2) {

                if (!WriteFile(hPipe, linha, (_tcslen(linha) + 1) * sizeof(TCHAR), &dwWritten, NULL)) {
                    Abort(_T("WriteFile failed.\n"));
                }
                if (!ReadFile(hPipe, resultado, sizeof(resultado), &dwRead, NULL) || dwRead == 0) {
                    Abort(_T("Houve um erro na leitura de mensagens do cliente.\n"));
                }

                MensagemInfo(resultado);
                _tprintf_s(_T("\n\n"));

                if (!_tcsicmp(resultado, _T("Login bem sucedido."))) {
                    loggedIn = true;
                }
            }
            else
                Erro(_T("Número de parâmetros inválido."));

        }
        else if (!_tcsicmp(comando, _T("listc"))) {

            if (nParam == 0) {

                if (!WriteFile(hPipe, linha, (_tcslen(linha) + 1) * sizeof(TCHAR), &dwWritten, NULL)) {
                    Abort(_T("WriteFile failed.\n"));
                }
                if (!ReadFile(hPipe, resultado, sizeof(resultado), &dwRead, NULL) || dwRead == 0) {
                    Abort(_T("Houve um erro na leitura de mensagens do cliente.\n"));
                }

                MP mp;

                if (!_tcsicmp(resultado, _T("listc"))) {
                    mp.hMapFile = OpenFileMapping(
                        FILE_MAP_ALL_ACCESS, // Read/write permission
                        FALSE,               // Do not inherit the name
                        TEXT("SharedMemory") // Name of the mapping object
                    );

                    if (mp.hMapFile == NULL) {
                        Abort(_T("Não foi possível abrir o objecto de mapeamento de memória partilhada.\n"));
                    }

                    // Map the shared memory into the process address space
                    mp.pBuf = (SharedData*)MapViewOfFile(mp.hMapFile,   // Handle to map object
                        FILE_MAP_ALL_ACCESS, // Read/write permission
                        0,
                        0,
                        0);

                    if (mp.pBuf == NULL) {
                        CloseHandle(mp.hMapFile);
                        Abort(_T("Não foi possível mapear o ficheiro.\n"));
                    }

                    if (mp.pBuf->numEmpresas == 0)
                        _tprintf_s(_T("\nNão existem empresas.\n\n"));

                    else {
                        _tprintf_s(_T("\n*********************************************************\n"));
                        _tprintf(_T("\nLista de Empresas:\n\n"));

                        for (DWORD i = 0; i < mp.pBuf->numEmpresas; i++) {
                            _tprintf(_T("Empresa %d:\n"), i + 1);
                            _tprintf(_T("  - Nome: %s\n"), mp.pBuf->empresas[i].nome);
                            _tprintf(_T("  - Número de ações: %u\n"), mp.pBuf->empresas[i].num_acoes);
                            _tprintf(_T("  - Preço da ação: %.2f\n\n"), mp.pBuf->empresas[i].preco_acao);
                        }
                        _tprintf_s(_T("*********************************************************\n\n"));
                    }

                }
                else if (!_tcsicmp(resultado, _T("Não existem empresas."))) {
                    MensagemInfo(resultado);
                    _tprintf_s(_T("\n\n"));
                }
                    
            }
            else
                Erro(_T("Número de parâmetros inválido."));

        }
        else if (!_tcsicmp(comando, _T("buy"))) {

            if (loggedIn == false) {
                Erro(_T("Login Obrigatório."));
            }
            else {
                if (nParam == 2) {

                    if (!WriteFile(hPipe, linha, (_tcslen(linha) + 1) * sizeof(TCHAR), &dwWritten, NULL)) {
                        Abort(_T("WriteFile failed.\n"));
                    }
                    if (!ReadFile(hPipe, resultado, sizeof(resultado), &dwRead, NULL) || dwRead == 0) {
                        Abort(_T("Houve um erro na leitura de mensagens do cliente.\n"));
                    }

                    MensagemInfo(resultado);
                    _tprintf_s(_T("\n\n"));
                }
                else
                    Erro(_T("Número de parâmetros inválido."));
            }
        }
        else if (!_tcsicmp(comando, _T("sell"))) {

            if (loggedIn == false) {
                Erro(_T("Login Obrigatório."));
            }
            else {
                if (nParam == 2) {

                    if (!WriteFile(hPipe, linha, (_tcslen(linha) + 1) * sizeof(TCHAR), &dwWritten, NULL)) {
                        Abort(_T("WriteFile failed.\n"));
                    }
                    if (!ReadFile(hPipe, resultado, sizeof(resultado), &dwRead, NULL) || dwRead == 0) {
                        Abort(_T("Houve um erro na leitura de mensagens do cliente.\n"));
                    }

                    MensagemInfo(resultado);
                    _tprintf_s(_T("\n\n"));
                }
                else
                    Erro(_T("Número de parâmetros inválido."));
            }
        }
        else if (!_tcsicmp(comando, _T("balance"))) {

            if (loggedIn == false) {
                Erro(_T("Login Obrigatório."));
            }
            else {
                if (nParam == 0) {

                    if (!WriteFile(hPipe, linha, (_tcslen(linha) + 1) * sizeof(TCHAR), &dwWritten, NULL)) {
                        Abort(_T("WriteFile failed.\n"));
                    }
                    if (!ReadFile(hPipe, resultado, sizeof(resultado), &dwRead, NULL) || dwRead == 0) {
                        Abort(_T("Houve um erro na leitura de mensagens do cliente.\n"));
                    }

                    MensagemInfo(resultado);
                    _tprintf_s(_T("\n\n"));
                }
                else
                    Erro(_T("Número de parâmetros inválido."));
            }
        }
        else if (!_tcsicmp(comando, _T("exit"))) {

            if (nParam == 0) {

                // Informa o servidor que vai encerrar
                if (!WriteFile(hPipe, linha, (_tcslen(linha) + 1) * sizeof(TCHAR), &dwWritten, NULL)) {
                    Abort(_T("WriteFile failed.\n"));
                }

                // Close pipe handle
                MensagemInfo(_T("A Encerrar o cliente.\n"));
                CloseHandle(hPipe);
                return 0;
            }
            else
                Erro(_T("Número de parâmetros inválido."));
        }
        else {
            Erro(_T("Comando Inválido."));
        }

    }

    // Close pipe handle
    CloseHandle(hPipe);
    return 1; // Não é suposto chegar aqui.
}