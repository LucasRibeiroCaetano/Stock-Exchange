#include "utils.h"


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
        // Se encontrarmos um caractere n�o espa�o em branco e n�o estamos dentro de um par�metro, incrementamos o contador
        if (linha[i] != _T(' ') && !dentroDoParametro) {
            dentroDoParametro = true;
            contador++;
        }
        // Se encontrarmos um espa�o em branco, definimos dentroDoParametro como falso
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

    // Extrair os par�metros
    for (int i = 0; i < numParametros; i++) {
        token = _tcstok_s(NULL, _T(" "), &context);
        if (token != NULL) {
            _tcscpy_s(parametros[i], STR_LEN, token);
        }
        else {
            parametros[i][0] = _T('\0'); // Se n�o houver mais tokens, colocamos uma string vazia
        }
    }
}

// Fun��o para imprimir uma mensagem com a parte "[INFO]" em ciano
void MensagemInfo(const TCHAR* mensagem) {
    _tprintf_s(_T("\n[\033[36mINFO\033[0m] %s"), mensagem);
}

// Thread para chamar as threads dos clientes
DWORD WINAPI ClienteThread(LPVOID lpParam) {

    DataClientes* dataCliente = (DataClientes*)lpParam;
    HANDLE hPipe = dataCliente->hPipe;
    HANDLE hSem = dataCliente->hSem;
    TCHAR buffer[1024];
    DWORD dwRead, dwWritten;
    TCHAR returnString[STR_LEN];


    // L� as mensagens dos clientes
    while (true) {
        if (!ReadFile(hPipe, buffer, sizeof(buffer), &dwRead, NULL) || dwRead == 0) {
            Abort(_T("Houve um erro na leitura de mensagens do cliente.\n"));
        }
        MensagemInfo(_T("Comando recebido: "));
        _tprintf_s(buffer);

        _tcscpy_s(returnString, STR_LEN, executaComandos(buffer, &dataCliente->activeUser, dataCliente->numUtilizadores, dataCliente->numEmpresas, dataCliente->empresas, dataCliente->utilizadores, dataCliente->carteiras, dataCliente->ultimaTransacao));

        if (returnString == NULL) {
            Erro(_T("\nErro ao executar o comando."));
        }

        if (!_tcsicmp(returnString, _T("EXIT"))) {

            // O utilizador n�o est� logado
            if (!_tcsicmp(dataCliente->activeUser, _T(""))) {
                // Fecha a handle
                CloseHandle(hPipe);
                return 0;
            }
            // Se o utilizador estiver logado, vamos meter o estado offline
            else if (dataCliente->utilizadores[getIndiceUtilizador(dataCliente->activeUser, dataCliente->utilizadores, dataCliente->numUtilizadores)].online == true) {
                dataCliente->utilizadores[getIndiceUtilizador(dataCliente->activeUser, dataCliente->utilizadores, dataCliente->numUtilizadores)].online = false;

                // Fecha a handle
                CloseHandle(hPipe);
                return 0;
            }
            else {
                Erro(_T("Erro inesperado."));
            }

        }

        // Comando inv�lido, n�o vamos enviar para o servidor
        if (!_tcsicmp(returnString, _T("Login Obrigat�rio."))) {
            MensagemInfo(_T("Comando inv�lido recebido."));
        }
        else {
            if (!WriteFile(hPipe, returnString, (_tcslen(returnString) + 1) * sizeof(TCHAR), &dwWritten, NULL)) {
                Abort(_T("WriteFile failed.\n"));
            }

        }
    }

    // Fecha a handle
    CloseHandle(hPipe);

    return 0;
}

void Erro(const TCHAR* mensagem) {
    _ftprintf(stderr, _T("\n\033[1;31m%s\033[0m\n\n"), mensagem);
}

TCHAR* executaComandos(TCHAR* linha, TCHAR* activeUser, DWORD numUtilizadores, DWORD numEmpresas, Empresa empresas[MAX_EMPRESAS], Utilizador utilizadores[MAX_USERS], CarteiraAcoes carteiras[MAX_USERS], UltimaTransacao ultimaTransacao) {

    TCHAR params[2][STR_LEN];
    TCHAR comando[STR_LEN];
    DWORD nParam = contaParametros(linha);
    TCHAR result[STR_LEN];
    extrairParametros(nParam, linha, comando, params);

    if (!_tcsicmp(comando, _T("login"))) {

        if (nParam == 2) {

            if (utilizadores[getIndiceUtilizador(params[0], utilizadores, numUtilizadores)].online) {
                return _T("O Utilizador j� se encontra online.");
            }

            for (DWORD i = 0; i < numUtilizadores; i++) {
                if (!_tcsicmp(params[0], utilizadores[i].username)) {
                    if (!_tcsicmp(params[1], utilizadores[i].password)) {
                        _tcscpy_s(activeUser, STR_LEN, params[0]);
                        utilizadores[i].online = true;

                        _tcscpy_s(result, STR_LEN, _T("Login bem sucedido."));
                        return result;
                    }
                }
            }

            return _T("Login mal sucedido. Tente novamente...");
        }
        else {
            return _T("N�mero de par�metros inv�lido.\n\n");
        }
    }

    else if ((!_tcsicmp(comando, _T("listc")))) {
        if (nParam == 0) {

            if (numEmpresas == 0)
                return _T("N�o existem empresas.");

            else {
                _tprintf_s(_T("\n*********************************************************\n"));
                _tprintf(_T("\nLista de Empresas:\n\n"));

                for (DWORD i = 0; i < numEmpresas; i++) {
                    _tprintf(_T("Empresa %d:\n"), i + 1);
                    _tprintf(_T("  - Nome: %s\n"), empresas[i].nome);
                    _tprintf(_T("  - N�mero de a��es: %u\n"), empresas[i].num_acoes);
                    _tprintf(_T("  - Pre�o da a��o: %.2f\n\n"), empresas[i].preco_acao);
                }
                _tprintf_s(_T("*********************************************************\n\n"));
                return NULL;
            }

        }
        else
            return _T("N�mero de par�metros inv�lido.\n\n");
    }

    else if ((!_tcsicmp(comando, _T("buy")))) {

        //Nome numAcoes
        DWORD numAcoes;
        DWORD indCarteira = getIndiceCarteira(activeUser, carteiras, numUtilizadores);
        DWORD indEmpresa;

        if (nParam == 2) {
            if (!_tcsicmp(activeUser, _T(""))) {
                return _T("Login Obrigat�rio.");
            }

            // O utilizador j� est� logado
            else {
                return _T("Sucesso!");

                /*
                // Encontra o nome da empresa
                for (DWORD i = 0; i < numEmpresas; i++) {
                    if (!_tcsicmp(params[0], empresas[i].nome)) {
                        indEmpresa = getIndiceEmpresa(empresas[i].nome, empresas, numEmpresas);
                        _stscanf_s(params[1], _T("%u"), &numAcoes);
                        empresas[i].num_acoes -= numAcoes;
                        _tcscpy_s(carteiras[indCarteira].empresas[carteiras[indCarteira].numEmpresas - 1].nome, STR_LEN, empresas[i].nome);
                        carteiras[indCarteira].empresas[carteiras[indCarteira].numEmpresas - 1].num_acoes += numAcoes;
                        carteiras[indCarteira].empresas[carteiras[indCarteira].numEmpresas - 1].preco_acao = empresas[indEmpresa].preco_acao;
                    }
                }
                */

            }


        }
        else {
            return _T("N�mero de par�metros inv�lido.");
        }
    }

    else if ((!_tcsicmp(comando, _T("sell")))) {
        if (nParam == 2) {

            if (!_tcsicmp(activeUser, _T(""))) {
                return _T("Login Obrigat�rio.");
            }

            // O utilizador j� est� logado
            else {
                return _T("Sucesso!");

                /*
                // Alterar a l�gica para venda
                for (DWORD i = 0; i < numEmpresas; i++) {
                    if (!_tcsicmp(params[0], empresas[i].nome)) {
                        indEmpresa = getIndiceEmpresa(empresas[i].nome, empresas, numEmpresas);
                        _stscanf_s(params[1], _T("%u"), &numAcoes);
                        empresas[i].num_acoes -= numAcoes;
                        _tcscpy_s(carteiras[indCarteira].empresas[carteiras[indCarteira].numEmpresas - 1].nome, STR_LEN, empresas[i].nome);
                        carteiras[indCarteira].empresas[carteiras[indCarteira].numEmpresas - 1].num_acoes += numAcoes;
                        carteiras[indCarteira].empresas[carteiras[indCarteira].numEmpresas - 1].preco_acao = empresas[indEmpresa].preco_acao;
                    }
                }
                */

            }

        }
        else {
            Erro(_T("N�mero de par�metros inv�lido."));
        }
    }
    else if ((!_tcsicmp(comando, _T("balance")))) {
        if (nParam == 0) {
            if (!_tcsicmp(activeUser, _T(""))) {
                return _T("Login Obrigat�rio.");
            }

            // O utilizador j� est� logado
            else {
                // Mostra o saldo do activeUser
                return _T("Sucesso!");
            }
        }
        else {
            Erro(_T("N�mero de par�metros inv�lido."));
        }

    }
    else if ((!_tcsicmp(comando, _T("exit")))) {
        if (nParam == 0) {
            // Libertar os recursos desse cliente??
            return _T("EXIT");
        }
        else {
            Erro(_T("N�mero de par�metros inv�lido."));
        }
    }
    else
        Erro(_T("Erro ao executar o comando."));
}

// Devolve o �ndice do utilizador
DWORD getIndiceUtilizador(TCHAR* username, Utilizador* utilizadores, DWORD numUtilizadores) {

    for (DWORD i = 0; i < numUtilizadores; i++) {
        if (!_tcsicmp(username, utilizadores[i].username)) {
            return i;
        }
    }
    return -1;
}

DWORD getIndiceCarteira(TCHAR* username, CarteiraAcoes* carteiras, DWORD numUtilizadores) {


    for (DWORD i = 0; i < numUtilizadores; i++) {
        if (!_tcsicmp(username, carteiras[i].username)) {
            return i;
        }
    }
    return -1;
}

DWORD getIndiceEmpresa(TCHAR* nome, Empresa* empresas, DWORD numEmpresas) {

    for (DWORD i = 0; i < numEmpresas; i++) {
        if (!_tcsicmp(nome, empresas[i].nome)) {
            return i;
        }
    }
    return -1;
}

// Verifica se o utilizador tem a empresa X na sua carteira de A��es
BOOL temEmpresa(TCHAR* activeUser, TCHAR* nome, CarteiraAcoes carteira) {
    if (carteira.numEmpresas == 0)
        return false;
    else {
        for (DWORD i = 0; i < carteira.numEmpresas; i++) {
            if (!_tcsicmp(nome, carteira.empresas[i].nome)) {
                return true;
            }
        }

        return false;
    }

}

