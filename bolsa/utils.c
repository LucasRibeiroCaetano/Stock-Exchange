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

// Thread para lidar com os comandos do administrador
DWORD WINAPI ComandosThread(LPVOID lpParam) {

    DataAdmin* dataAdmin = (DataAdmin*)lpParam;

    TCHAR comando[STR_LEN];
    TCHAR linhaAUX[STR_LEN];
    TCHAR linha[STR_LEN];
    DWORD nParam;
    TCHAR param[MAX_PARAM][STR_LEN];
    DWORD nSegundos = 0;
    FILE* file;

    // Tenho de enviar array empresas, numEmpresas, ultimaTransacao, eventos, utilizadores, numUtilizadores

    // Interface de gest�o de comandos
    _tprintf(_T("Escreva 'ajuda' para uma lista completa de comandos."));

    while (1) {

        _tprintf(_T("\n\nAdministrador> "));

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
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));
        }

        // Adicionar uma empresa
        else if (!_tcsicmp(comando, _T("addc"))) {
            if (nParam == 3) {

                TCHAR nome[STR_LEN];
                int num_acoes;
                float preco_acao;

                if (_stscanf_s(linha, _T("%*s %s %d %f"), nome, STR_LEN, &num_acoes, &preco_acao) != 3) {
                    Abort(_T("Erro ao extrair informa��es da empresa.\n"));
                }

                // Armazenar informa��es na estrutura
                _tcscpy_s(dataAdmin->empresas[dataAdmin->numEmpresas].nome, STR_LEN, nome);
                dataAdmin->empresas[dataAdmin->numEmpresas].num_acoes = num_acoes;
                dataAdmin->empresas[dataAdmin->numEmpresas].preco_acao = preco_acao;

                dataAdmin->numEmpresas++;

                MensagemInfo(_T("A atualizar a informa��o..."));

                atualizarBoard(dataAdmin->mp, dataAdmin->empresas, dataAdmin->numEmpresas, dataAdmin->ultimaTransacao);

                SetEvent(dataAdmin->eventos.hRead);
                MensagemInfo(_T("Evento de leitura ligado."));
                ResetEvent(dataAdmin->eventos.hRead);
                MensagemInfo(_T("Evento de leitura desligado."));
            }
            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));

        }

        // Adicionar empresas por ficheiro
        else if (!_tcsicmp(comando, _T("addf"))) {
            if (nParam == 1) {

                TCHAR nomeFich[STR_LEN];
                TCHAR linhaFich[STR_LEN];
                _tcscpy_s(nomeFich, STR_LEN, param[0]);

                // Abrir o ficheiro
                if (_tfopen_s(&file, nomeFich, _T("r, ccs=UTF-8")) != 0 || file == NULL) {
                    Abort(_T("Falha ao abrir o arquivo.\n"));
                }

                // Ler as empresas do ficheiro
                while (dataAdmin->numEmpresas < MAX_EMPRESAS && _fgetts(linhaFich, STR_LEN, file)) {
                    TCHAR nome[STR_LEN];
                    int num_acoes;
                    float preco_acao;

                    // Extrair informa��es da linha
                    if (_stscanf_s(linhaFich, _T("%s %d %f"), nome, STR_LEN, &num_acoes, &preco_acao) != 3) {
                        fclose(file);
                        Abort(_T("Erro ao extrair informa��es da empresa do arquivo.\n"));
                    }

                    // Copiar as informa��es para a estrutura de dados
                    _tcscpy_s(dataAdmin->empresas[dataAdmin->numEmpresas].nome, STR_LEN, nome);
                    dataAdmin->empresas[dataAdmin->numEmpresas].num_acoes = num_acoes;
                    dataAdmin->empresas[dataAdmin->numEmpresas].preco_acao = preco_acao;

                    dataAdmin->numEmpresas++;

                }

                fclose(file);

                MensagemInfo(_T("A atualizar a informa��o..."));

                atualizarBoard(dataAdmin->mp, dataAdmin->empresas, dataAdmin->numEmpresas, dataAdmin->ultimaTransacao);

                SetEvent(dataAdmin->eventos.hRead);
                MensagemInfo(_T("Evento de leitura ligado."));
                ResetEvent(dataAdmin->eventos.hRead);
                MensagemInfo(_T("Evento de leitura desligado."));
            }
            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));

        }

        // Listar todas as empresas
        else if (!_tcsicmp(comando, _T("listc"))) {
            if (nParam == 0) {

                if (dataAdmin->numEmpresas == 0)
                    _tprintf_s(_T("\nN�o existem empresas.\n\n"));

                else {
                    _tprintf_s(_T("\n*********************************************************\n"));
                    _tprintf(_T("\nLista de Empresas:\n\n"));

                    for (DWORD i = 0; i < dataAdmin->numEmpresas; i++) {
                        _tprintf(_T("Empresa %d:\n"), i + 1);
                        _tprintf(_T("  - Nome: %s\n"), dataAdmin->empresas[i].nome);
                        _tprintf(_T("  - N�mero de a��es: %u\n"), dataAdmin->empresas[i].num_acoes);
                        _tprintf(_T("  - Pre�o da a��o: %.2f\n\n"), dataAdmin->empresas[i].preco_acao);
                    }
                    _tprintf_s(_T("*********************************************************\n\n"));
                }

            }
            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));

        }

        // Redefinir custo das a��es de uma empresa
        else if (!_tcscmp(comando, _T("stock"))) {

            if (nParam == 2) {

                for (DWORD i = 0; i < dataAdmin->numEmpresas; i++) {
                    if (!_tcscmp(dataAdmin->empresas[i].nome, param[0])) {
                        if (_stscanf_s(param[1], _T("%f"), &dataAdmin->empresas[i].preco_acao) != 1) {
                            Abort(_T("Erro na convers�o string -> float."));
                        }
                    }
                }
            }

            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));
        }

        // Listar Utilizadores
        else if (!_tcsicmp(comando, _T("users"))) {

            if (nParam == 0) {
                _tprintf_s(_T("\n*********************************************************\n\n"));

                for (DWORD i = 0; i < dataAdmin->numUtilizadores; i++) {
                    _tprintf(_T("%s %s:\n"), dataAdmin->utilizadores[i].online ? _T("\033[32m\u25CF\033[0m") : _T("\033[31m\u25CF\033[0m"), dataAdmin->utilizadores[i].username);
                    _tprintf(_T("    Saldo:    %.2f �\n\n"), dataAdmin->utilizadores[i].saldo);
                }

                _tprintf_s(_T("*********************************************************\n\n"));

            }
            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));
        }

        // Pausar Opera��es
        else if (!_tcsicmp(comando, _T("pause"))) {

            if (nParam == 1) {

                if (_stscanf_s(param[1], _T("%u"), &nSegundos) != 1) {
                    Abort(_T("Erro na leitura de segundos."));
                }

                // Aciona uma flag que bloqueia todas as compras e vendas

            }

            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));
        }

        // Limpar a Consola
        else if (!_tcsicmp(comando, _T("limpar"))) {

            if (nParam == 0)
                limparConsola();
            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));
        }

        // Sair do programa
        else if (_tcsicmp(comando, _T("close")) == 0) {

            if (nParam == 0) {
                _tprintf(_T("\nEncerrando o programa...\n\n"));
                SetEvent(dataAdmin->hClose);
                return 0;
            }
            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));
        }

        else {
            _tprintf(_T("\nComando n�o reconhecido... Escreva 'ajuda' para uma lista completa de comandos. \n\n"));
        }
    }


}

// Thread para chamar as threads dos clientes
DWORD WINAPI ClientesThread(LPVOID lpParam) {

    DataAdmin* dataAdmin = (DataAdmin*)lpParam;
    TCHAR buffer[1024];
    DWORD dwRead, dwWritten;
    TCHAR returnString[STR_LEN];
    DWORD id = dataAdmin->dataClientes.idPipe;
    HANDLE hThread;
    DWORD dwThreadId;

    DWORD i = 0;

    // L� as mensagens dos clientes
    while (true) {

        // Check if hClose has finished, if it hasn't finished yet
        DWORD dwWaitClose = WaitForSingleObject(dataAdmin->hClose, 0);

        if (dwWaitClose == WAIT_OBJECT_0) {
            // Sair do ciclo e encerrar o programa
            return 0;
        }
        else if (dwWaitClose == WAIT_TIMEOUT);
        else {
            // Error occurred
            Erro(_T("\nErro ao esperar pelo evento hClose\n"));
        }

        // L� o comando do cliente       
        if (!ReadFile(dataAdmin->hPipes[id], buffer, sizeof(buffer), &dwRead, NULL) || dwRead == 0) {
            Abort(_T("Houve um erro na leitura de mensagens do cliente.\n"));
        }

        MensagemInfo(_T("Comando recebido: "));
        _tprintf_s(buffer);

        _tcscpy_s(returnString, STR_LEN, executaComandos(buffer, dataAdmin->dataClientes.activeUser, dataAdmin->numUtilizadores, dataAdmin->numEmpresas, dataAdmin->empresas, dataAdmin->utilizadores, dataAdmin->carteiras, dataAdmin->ultimaTransacao, dataAdmin->hSem));

        if (returnString == NULL) {
            Erro(_T("\nErro ao executar o comando."));
        }

        if (!_tcsicmp(returnString, _T("EXIT"))) {

            // O utilizador n�o est� logado
            if (!_tcsicmp(dataAdmin->dataClientes.activeUser, _T(""))) {

                CloseHandle(dataAdmin->hPipes[id]);
                dataAdmin->hPipes[id] = NULL;
                return 0;
            }
            // Se o utilizador estiver logado, vamos meter o estado offline
            else if (dataAdmin->utilizadores[getIndiceUtilizador(dataAdmin->dataClientes.activeUser, dataAdmin->utilizadores, dataAdmin->numUtilizadores)].online == true) {
                dataAdmin->utilizadores[getIndiceUtilizador(dataAdmin->dataClientes.activeUser, dataAdmin->utilizadores, dataAdmin->numUtilizadores)].online = false;

                CloseHandle(dataAdmin->hPipes[id]);
                dataAdmin->hPipes[id] = NULL;
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

            // Envia o resultado do comando enviado pelo cliente
            if (!WriteFile(dataAdmin->hPipes[id], returnString, (_tcslen(returnString) + 1) * sizeof(TCHAR), &dwWritten, NULL)) {
                Abort(_T("WriteFile failed.\n"));
            }

        }
    }
}

void Erro(const TCHAR* mensagem) {
    _ftprintf(stderr, _T("\n\033[1;31m%s\033[0m\n\n"), mensagem);
}

TCHAR* executaComandos(TCHAR* linha, TCHAR* activeUser, DWORD numUtilizadores, DWORD numEmpresas, Empresa empresas[MAX_EMPRESAS], Utilizador utilizadores[MAX_USERS], CarteiraAcoes carteiras[MAX_USERS], UltimaTransacao ultimaTransacao, HANDLE hSem) {

    TCHAR params[2][STR_LEN];
    TCHAR comando[STR_LEN];
    DWORD nParam = contaParametros(linha);
    TCHAR result[STR_LEN];
    DWORD indice = 0;
    extrairParametros(nParam, linha, comando, params);

    if (!_tcsicmp(comando, _T("login"))) {

        if (nParam == 2) {

            indice = getIndiceUtilizador(params[0], utilizadores, numUtilizadores);

            if (indice == - 1) {
                return _T("Utilizador n�o existe.");
            }

            if (utilizadores[indice].online) {
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
                return _T("listc");
            }

        }
        else
            return _T("N�mero de par�metros inv�lido.\n\n");
    }

    else if ((!_tcsicmp(comando, _T("buy")))) {

        //Nome numAcoes
        int numAcoes;
        float disponibilidade;
        DWORD indCarteira = getIndiceCarteira(activeUser, carteiras, numUtilizadores);

        // Se indEmpresa == -1, esta empresa n�o existe
        DWORD indEmpresa = getIndiceEmpresa(params[0], empresas, numEmpresas);

        // Se indEmpresaCarteira == -1, o utilizador n�o tem esta empresa na carteira
        DWORD indEmpresaCarteira = getIndiceEmpresaCarteira(activeUser, params[0], carteiras, numUtilizadores, carteiras[indCarteira].numEmpresas, indCarteira);

        DWORD indUtilizador = getIndiceUtilizador(activeUser, utilizadores, numUtilizadores);
        float custo = 0;

        _stscanf_s(params[1], _T("%d"), &numAcoes);

        if (nParam == 2) {
            if (!_tcsicmp(activeUser, _T(""))) {
                return _T("Login Obrigat�rio.");
            }

            // O utilizador j� est� logado
            else {

                if (indUtilizador == -1) {
                    return _T("Utilizador n�o encontrado.");
                }

                if (indCarteira == -1) {
                    return _T("Carteira n�o encontrada.");
                }

                custo = empresas[indEmpresa].preco_acao * numAcoes;

                if (utilizadores[indUtilizador].saldo < custo) {
                    return _T("N�o tem saldo suficiente.");
                }

                // Verificar se a empresa existe
                if (indEmpresa == -1)
                    return _T("Nome da empresa n�o encontrado.");

                empresas[indEmpresa].num_acoes -= numAcoes;

                _tcscpy_s(ultimaTransacao.nome, STR_LEN, activeUser);
                ultimaTransacao.num_acoes = numAcoes;
                ultimaTransacao.preco_acao = empresas[indEmpresa].preco_acao;

                // O utilizador n�o tem a��es desta empresa na carteira, o �ndice � numEmpresas
                if (indEmpresaCarteira == -1) {

                    indEmpresaCarteira = carteiras[indCarteira].numEmpresas;
                    _tcscpy_s(carteiras[indCarteira].empresas[indEmpresaCarteira].nome, STR_LEN, empresas[indEmpresa].nome);
                    carteiras[indCarteira].empresas[indEmpresaCarteira].num_acoes = numAcoes;
                    carteiras[indCarteira].empresas[indEmpresaCarteira].preco_acao = empresas[indEmpresa].preco_acao;
                    carteiras[indCarteira].numEmpresas++;
                }

                // O utilizador j� tem a��es desta empresa
                else {
                    _tcscpy_s(carteiras[indCarteira].empresas[indEmpresaCarteira].nome, STR_LEN, empresas[indEmpresa].nome);
                    carteiras[indCarteira].empresas[indEmpresaCarteira].num_acoes += numAcoes;
                    carteiras[indCarteira].empresas[indEmpresaCarteira].preco_acao = empresas[indEmpresa].preco_acao;
                }

                utilizadores[indUtilizador].saldo -= custo;

                disponibilidade = (float)numAcoes / empresas[indEmpresa].num_acoes; // Valor entre 0 e 1

                empresas[indEmpresa].preco_acao += disponibilidade * empresas[indEmpresa].preco_acao;

                return _T("Compra bem sucedida.\n");
            }


        }
        else {
            return _T("N�mero de par�metros inv�lido.");
        }
    }

    else if ((!_tcsicmp(comando, _T("sell")))) {

        //Nome numAcoes
        DWORD numAcoes;
        float disponibilidade;
        DWORD indCarteira = getIndiceCarteira(activeUser, carteiras, numUtilizadores);

        // Se indEmpresa == -1, esta empresa n�o existe
        DWORD indEmpresa = getIndiceEmpresa(params[0], empresas, numEmpresas);

        // Se indEmpresaCarteira == -1, o utilizador n�o tem esta empresa na carteira
        DWORD indEmpresaCarteira = getIndiceEmpresaCarteira(activeUser, params[0], carteiras, numUtilizadores, carteiras[indCarteira].numEmpresas, indCarteira);

        DWORD indUtilizador = getIndiceUtilizador(activeUser, utilizadores, numUtilizadores);
        float lucro = 0;
        _stscanf_s(params[1], _T("%u"), &numAcoes);

        if (nParam == 2) {

            if (!_tcsicmp(activeUser, _T(""))) {
                return _T("Login Obrigat�rio.");
            }

            // O utilizador j� est� logado
            else {

                if (indUtilizador == -1) {
                    return _T("Utilizador n�o encontrado.");
                }

                if (indCarteira == -1) {
                    return _T("Carteira n�o encontrada.");
                }

                if (indEmpresaCarteira == -1) {
                    return _T("N�o tem a��es dessa empresa.");
                }

                lucro = empresas[indEmpresa].preco_acao * numAcoes;

                // Tem a��es suficientes para vender?
                if (carteiras[indCarteira].empresas[indEmpresaCarteira].num_acoes < numAcoes) {
                    return _T("N�o tens a��es suficientes.");
                }

                _tcscpy_s(ultimaTransacao.nome, STR_LEN, activeUser);
                ultimaTransacao.num_acoes = numAcoes;
                ultimaTransacao.preco_acao = empresas[indEmpresa].preco_acao;

                // Adicionar as a��es ao mercado
                empresas[indEmpresa].num_acoes += numAcoes;

                // Retirar as a��es do utilizador
                carteiras[indCarteira].empresas[indEmpresaCarteira].num_acoes -= numAcoes;

                // Adicionar o dinheiro
                utilizadores[indUtilizador].saldo += lucro;

                disponibilidade = (float)numAcoes / empresas[indEmpresa].num_acoes; // Valor entre 0 e 1

                empresas[indEmpresa].preco_acao -= disponibilidade * empresas[indEmpresa].preco_acao;

                return _T("Venda bem sucedida.\n");
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

            ReleaseSemaphore(hSem, 1, NULL);

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

    // Utilizador n�o encontrado
    return -1;
}

DWORD getIndiceCarteira(TCHAR* username, CarteiraAcoes* carteiras, DWORD numUtilizadores) {

    for (DWORD i = 0; i < numUtilizadores; i++) {
        if (!_tcsicmp(username, carteiras[i].username)) {
            return i;
        }
    }

    // Este utilizador n�o tem carteira
    return -1;
}

// Fun��o para devolver o �ndice da empresa X na carteira do utilizador
DWORD getIndiceEmpresaCarteira(TCHAR* activeUser, TCHAR* nomeEmpresa, CarteiraAcoes* carteiras, DWORD numUtilizadores, DWORD numEmpresas, DWORD indCarteira) {

    if (numEmpresas == 0)
        return -1;

    
    for (DWORD i = 0; i < numEmpresas; i++) {
        _tprintf_s(_T("ActiveUser: %s|nome empresa a encontrar: %s==%s"), activeUser, nomeEmpresa, carteiras[indCarteira].empresas[i].nome);
        if (!_tcsicmp(nomeEmpresa, carteiras[indCarteira].empresas[i].nome)) {
            return i;
        }
    }

    // O utilizador n�o tem esta empresa na carteira
    return -1;
}

DWORD getIndiceEmpresa(TCHAR* nome, Empresa* empresas, DWORD numEmpresas) {

    for (DWORD i = 0; i < numEmpresas; i++) {
        if (!_tcsicmp(nome, empresas[i].nome)) {
            return i;
        }
    }

    // Esta empresa n�o existe
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

// Function to return the index of a free pipe
DWORD getPipe(HANDLE* hPipes, DWORD numPipes) {
    for (DWORD i = 0; i < numPipes; i++) {
        if (hPipes[i] == NULL) {
            return i;
        }
    }

    // Devolve -1 se n�o houver pipe livre
    return (DWORD)-1;
}

void inicializaPipes(HANDLE* hPipes, DWORD numPipes) {
    for (DWORD i = 0; i < numPipes; i++) {
        hPipes[i] = NULL;
    }
}

