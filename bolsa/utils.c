#include "utils.h"
#include "data.h"


void Abort(const TCHAR* message) {
    DWORD errorCode = GetLastError(); // Retrieve the error code
    LPVOID lpMsgBuf; // Buffer to hold error message
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
    );

    // Print error message along with the custom message
    _ftprintf(stderr, _T("\n\033[1;31m%s\033[0m"), message);
    _ftprintf(stderr, _T("\033[1;31mDetalhes do erro: %s\033[0m\n"), (LPCTSTR)lpMsgBuf);

    // Free the buffer
    LocalFree(lpMsgBuf);

    exit(EXIT_FAILURE);
}

void limparConsola() {
    system("cls");
}

int contaParametros(const TCHAR* linha) {
    int contador = 0;
    bool dentroDoParametro = false;

    // Percorre cada caractere da linha
    for (int i = 0; linha[i] != _T('\0'); i++) {
        // Se encontrarmos um caractere não espaço em branco e não estamos dentro de um parâmetro, incrementamos o contador
        if (linha[i] != _T(' ') && !dentroDoParametro) {
            dentroDoParametro = true;
            contador++;
        }
        // Se encontrarmos um espaço em branco, definimos dentroDoParametro como falso
        else if (linha[i] == _T(' ')) {
            dentroDoParametro = false;
        }
    }

    return contador - 1;
}

void extrairParametros(int numParametros, const TCHAR* linha, TCHAR comando[], TCHAR parametros[][STR_LEN]) {
    TCHAR* context;
    TCHAR* token;

    // Extrair o comando
    token = _tcstok_s(linha, _T(" "), &context);
    if (token != NULL) {
        _tcscpy_s(comando, STR_LEN, token);
    }

    // Extrair os parâmetros
    for (int i = 0; i < numParametros; i++) {
        token = _tcstok_s(NULL, _T(" "), &context);
        if (token != NULL) {
            _tcscpy_s(parametros[i], STR_LEN, token);
        }
        else {
            parametros[i][0] = _T('\0'); // Se não houver mais tokens, colocamos uma string vazia
        }
    }
}

// Função para imprimir uma mensagem com a parte "[INFO]" em ciano
void MensagemInfo(const TCHAR* mensagem) {
    _tprintf_s(_T("\n[\033[36mINFO\033[0m] %s"), mensagem);
}

