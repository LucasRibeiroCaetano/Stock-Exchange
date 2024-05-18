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

    // Para validar as opera��es que precisam de login
    DWORD loggedIn = false;

    DWORD stdinReturn;
    DWORD stdoutReturn;
    DWORD stderrReturn;

    HANDLE hPipe;

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

    limparConsola();

    // Interface de gest�o de comandos
    _tprintf(_T("Escreva 'ajuda' para uma lista completa de comandos.\n"));

    // Ciclo de enviar comandos, copiar da bolsa
    while (1) {

        _tprintf(_T("Cliente> "));

        _fgetts(linhaAUX, STR_LEN, stdin);

        // Obt�m o tamanho do comando
        size_t length = _tcslen(linhaAUX);

        // Se o comando n�o estiver vazio truncar o \n
        if (length > 0 && linhaAUX[length - 1] == '\n') {
            linhaAUX[length - 1] = '\0';
        }

        // linhaAUX ir� ser alterado nas fun��es ent�o copio o conte�do para outra vari�vel para guardar o input
        _tcscpy_s(linha, STR_LEN, linhaAUX);

        nParam = contaParametros(linhaAUX);

        // Repartir o comando e os par�metros
        extrairParametros(nParam, linhaAUX, comando, param);

        if (!_tcsicmp(comando, _T("ajuda"))) {

            if (nParam == 0) {
                _tprintf_s(_T("\n*********************************************************\n\n"));
                _tprintf(_T("Acrescentar uma empresa\n - addc <nome-empresa> <n�mero-a��es> <pre�o-a��o>\n\n"));
                _tprintf(_T("Ler as empresas de um ficheiro de texto\n - addf <nome-ficheiro>\n\n"));
                _tprintf(_T("Listar todas as empresas\n - listc\n\n"));
                _tprintf(_T("Redefinir custo das a��es de uma empresa\n - stock <nome-empresa> <pre�o-a��o>\n\n"));
                _tprintf(_T("Listar utilizadores\n - users\n\n"));
                _tprintf(_T("Pausar as opera��es de compra e venda\n - pause <n�mero-segundos>\n\n"));
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
                Erro(_T("Número de parâmetros inv�lido."));

        }
        else if (!_tcsicmp(comando, _T("listc"))) {

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
                Erro(_T("Login Obrigat�rio."));
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
                    Erro(_T("N�mero de parâmetros inválido."));
            }
        }
        else if (!_tcsicmp(comando, _T("exit"))) {

            if (nParam == 0) {

                // Informa o servidor que vai encerrar
                if (!WriteFile(hPipe, linha, (_tcslen(linha) + 1) * sizeof(TCHAR), &dwWritten, NULL)) {
                    Abort(_T("WriteFile failed.\n"));
                }

                // Close pipe handle
                MensagemInfo(_T("A Encerrar o cliente."));
                CloseHandle(hPipe);
                return 0;
            }
            else
                Erro(_T("N�mero de parâmetros inv�lido."));
        }
        else {
            Erro(_T("Comando Inválido."));
        }

    }

    // Close pipe handle
    CloseHandle(hPipe);
    return 1; // Não é suposto chegar aqui.
}