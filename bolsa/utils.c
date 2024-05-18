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

    // Interface de gestão de comandos
    _tprintf(_T("Escreva 'ajuda' para uma lista completa de comandos."));

    while (1) {
        _tprintf(_T("\n\nAdministrador> "));

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
                _tprintf(_T("Acrescentar uma empresa\n - addc <nome-empresa> <número-ações> <preço-ação>\n\n"));
                _tprintf(_T("Ler as empresas de um ficheiro de texto\n - addf <nome-ficheiro>\n\n"));
                _tprintf(_T("Listar todas as empresas\n - listc\n\n"));
                _tprintf(_T("Redefinir custo das ações de uma empresa\n - stock <nome-empresa> <preço-ação>\n\n"));
                _tprintf(_T("Listar utilizadores\n - users\n\n"));
                _tprintf(_T("Pausar as operações de compra e venda\n - pause <número-segundos>\n\n"));
                _tprintf(_T("Limpar a consola\n - limpar\n\n"));
                _tprintf(_T("Encerrar a plataforma\n - close\n\n"));
                _tprintf_s(_T("*********************************************************\n\n"));
            }
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));
        }

        // Adicionar uma empresa
        else if (!_tcsicmp(comando, _T("addc"))) {
            if (nParam == 3) {

                TCHAR nome[STR_LEN];
                int num_acoes;
                float preco_acao;

                if (_stscanf_s(linha, _T("%*s %s %d %f"), nome, STR_LEN, &num_acoes, &preco_acao) != 3) {
                    Abort(_T("Erro ao extrair informações da empresa.\n"));
                }

                // Armazenar informações na estrutura
                _tcscpy_s(dataAdmin->empresas[dataAdmin->numEmpresas].nome, STR_LEN, nome);
                dataAdmin->empresas[dataAdmin->numEmpresas].num_acoes = num_acoes;
                dataAdmin->empresas[dataAdmin->numEmpresas].preco_acao = preco_acao;

                dataAdmin->numEmpresas++;

                MensagemInfo(_T("A atualizar a informação..."));

                atualizarBoard(dataAdmin->mp, dataAdmin->empresas, dataAdmin->numEmpresas, dataAdmin->ultimaTransacao);

                SetEvent(dataAdmin->eventos.hRead);
                MensagemInfo(_T("Evento de leitura ligado."));
                ResetEvent(dataAdmin->eventos.hRead);
                MensagemInfo(_T("Evento de leitura desligado."));
            }
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));

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

                    // Extrair informações da linha
                    if (_stscanf_s(linhaFich, _T("%s %d %f"), nome, STR_LEN, &num_acoes, &preco_acao) != 3) {
                        fclose(file);
                        Abort(_T("Erro ao extrair informações da empresa do arquivo.\n"));
                    }

                    // Copiar as informações para a estrutura de dados
                    _tcscpy_s(dataAdmin->empresas[dataAdmin->numEmpresas].nome, STR_LEN, nome);
                    dataAdmin->empresas[dataAdmin->numEmpresas].num_acoes = num_acoes;
                    dataAdmin->empresas[dataAdmin->numEmpresas].preco_acao = preco_acao;

                    dataAdmin->numEmpresas++;

                }

                fclose(file);

                MensagemInfo(_T("A atualizar a informação..."));

                atualizarBoard(dataAdmin->mp, dataAdmin->empresas, dataAdmin->numEmpresas, dataAdmin->ultimaTransacao);

                SetEvent(dataAdmin->eventos.hRead);
                MensagemInfo(_T("Evento de leitura ligado."));
                ResetEvent(dataAdmin->eventos.hRead);
                MensagemInfo(_T("Evento de leitura desligado."));
            }
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));

        }

        // Listar todas as empresas
        else if (!_tcsicmp(comando, _T("listc"))) {
            if (nParam == 0) {

                if (dataAdmin->numEmpresas == 0)
                    _tprintf_s(_T("\nNão existem empresas.\n\n"));

                else {
                    _tprintf_s(_T("\n*********************************************************\n"));
                    _tprintf(_T("\nLista de Empresas:\n\n"));

                    for (DWORD i = 0; i < dataAdmin->numEmpresas; i++) {
                        _tprintf(_T("Empresa %d:\n"), i + 1);
                        _tprintf(_T("  - Nome: %s\n"), dataAdmin->empresas[i].nome);
                        _tprintf(_T("  - Número de ações: %u\n"), dataAdmin->empresas[i].num_acoes);
                        _tprintf(_T("  - Preço da ação: %.2f\n\n"), dataAdmin->empresas[i].preco_acao);
                    }
                    _tprintf_s(_T("*********************************************************\n\n"));
                }

            }
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));

        }

        // Redefinir custo das ações de uma empresa
        else if (!_tcscmp(comando, _T("stock"))) {

            if (nParam == 2) {

                for (DWORD i = 0; i < dataAdmin->numEmpresas; i++) {
                    if (!_tcscmp(dataAdmin->empresas[i].nome, param[0])) {
                        if (_stscanf_s(param[1], _T("%f"), &dataAdmin->empresas[i].preco_acao) != 1) {
                            Abort(_T("Erro na conversão string -> float."));
                        }
                    }
                }
            }

            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));
        }

        // Listar Utilizadores
        else if (!_tcsicmp(comando, _T("users"))) {

            if (nParam == 0) {
                _tprintf_s(_T("\n*********************************************************\n\n"));

                for (DWORD i = 0; i < dataAdmin->numUtilizadores; i++) {
                    _tprintf(_T("%s %s:\n"), dataAdmin->utilizadores[i].online ? _T("\033[32m\u25CF\033[0m") : _T("\033[31m\u25CF\033[0m"), dataAdmin->utilizadores[i].username);
                    _tprintf(_T("    Saldo:    %.2f €\n\n"), dataAdmin->utilizadores[i].saldo);
                }

                _tprintf_s(_T("*********************************************************\n\n"));

            }
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));
        }

        // Pausar Operações
        else if (!_tcsicmp(comando, _T("pause"))) {

            if (nParam == 1) {

                if (_stscanf_s(param[1], _T("%u"), &nSegundos) != 1) {
                    Abort(_T("Erro na leitura de segundos."));
                }

                // Aciona uma flag que bloqueia todas as compras e vendas

            }

            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));
        }

        // Limpar a Consola
        else if (!_tcsicmp(comando, _T("limpar"))) {

            if (nParam == 0)
                limparConsola();
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));
        }

        // Sair do programa
        else if (_tcsicmp(comando, _T("close")) == 0) {

            if (nParam == 0) {
                _tprintf(_T("\nEncerrando o programa...\n\n"));
                return 1;
            }
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));
        }

        else {
            _tprintf(_T("\nComando não reconhecido... Escreva 'ajuda' para uma lista completa de comandos. \n\n"));
        }
    }


}

// Thread para chamar as threads dos clientes
DWORD WINAPI ClientesThread(LPVOID lpParam) {

    DataAdmin* dataAdmin = (DataAdmin*)lpParam;
    TCHAR buffer[1024];
    DWORD dwRead, dwWritten;
    TCHAR returnString[STR_LEN];

    // Read file vai ser dataAdmin->dataCliente->hPipe[dataAdmin->dataCliente->idPipe]

    // Lê as mensagens dos clientes
    while (true) {

        if (!ReadFile(dataAdmin->hPipes[dataAdmin->dataClientes.idPipe], buffer, sizeof(buffer), &dwRead, NULL) || dwRead == 0) {
            Abort(_T("Houve um erro na leitura de mensagens do cliente.\n"));
        }
        MensagemInfo(_T("Comando recebido: "));
        _tprintf_s(buffer);

        _tcscpy_s(returnString, STR_LEN, executaComandos(buffer, dataAdmin->dataClientes.activeUser, dataAdmin->numUtilizadores, dataAdmin->numEmpresas, dataAdmin->empresas, dataAdmin->utilizadores, dataAdmin->carteiras, dataAdmin->ultimaTransacao));

        if (returnString == NULL) {
            Erro(_T("\nErro ao executar o comando."));
        }

        if (!_tcsicmp(returnString, _T("EXIT"))) {

            // O utilizador não está logado
            if (!_tcsicmp(dataAdmin->dataClientes.activeUser, _T(""))) {

                CloseHandle(dataAdmin->hPipes[dataAdmin->dataClientes.idPipe]);
                dataAdmin->hPipes[dataAdmin->dataClientes.idPipe] = NULL;
                return 0;
            }
            // Se o utilizador estiver logado, vamos meter o estado offline
            else if (dataAdmin->utilizadores[getIndiceUtilizador(dataAdmin->dataClientes.activeUser, dataAdmin->utilizadores, dataAdmin->numUtilizadores)].online == true) {
                dataAdmin->utilizadores[getIndiceUtilizador(dataAdmin->dataClientes.activeUser, dataAdmin->utilizadores, dataAdmin->numUtilizadores)].online = false;

                CloseHandle(dataAdmin->hPipes[dataAdmin->dataClientes.idPipe]);
                dataAdmin->hPipes[dataAdmin->dataClientes.idPipe] = NULL;
                return 0;
            }
            else {
                Erro(_T("Erro inesperado."));
            }

        }

        _tprintf_s(_T("\n3\n"));

        // Comando inválido, não vamos enviar para o servidor
        if (!_tcsicmp(returnString, _T("Login Obrigatório."))) {
            MensagemInfo(_T("Comando inválido recebido."));
        }
        else {
            if (!WriteFile(dataAdmin->hPipes[dataAdmin->dataClientes.idPipe], returnString, (_tcslen(returnString) + 1) * sizeof(TCHAR), &dwWritten, NULL)) {
                Abort(_T("WriteFile failed.\n"));
            }

        }
    }
}

void Erro(const TCHAR* mensagem) {
    _ftprintf(stderr, _T("\n\033[1;31m%s\033[0m\n\n"), mensagem);
}

TCHAR* executaComandos(TCHAR* linha, TCHAR* activeUser, DWORD numUtilizadores, DWORD numEmpresas, Empresa empresas[MAX_EMPRESAS], Utilizador utilizadores[MAX_USERS], CarteiraAcoes carteiras[MAX_USERS], UltimaTransacao ultimaTransacao) {

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
                return _T("Utilizador não existe.");
            }

            if (utilizadores[indice].online) {
                return _T("O Utilizador já se encontra online.");
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
            return _T("Número de parâmetros inválido.\n\n");
        }
    }

    else if ((!_tcsicmp(comando, _T("listc")))) {
        if (nParam == 0) {

            if (numEmpresas == 0)
                return _T("Não existem empresas.");

            else {
                _tprintf_s(_T("\n*********************************************************\n"));
                _tprintf(_T("\nLista de Empresas:\n\n"));

                for (DWORD i = 0; i < numEmpresas; i++) {
                    _tprintf(_T("Empresa %d:\n"), i + 1);
                    _tprintf(_T("  - Nome: %s\n"), empresas[i].nome);
                    _tprintf(_T("  - Número de ações: %u\n"), empresas[i].num_acoes);
                    _tprintf(_T("  - Preço da ação: %.2f\n\n"), empresas[i].preco_acao);
                }
                _tprintf_s(_T("*********************************************************\n\n"));
                return NULL;
            }

        }
        else
            return _T("Número de parâmetros inválido.\n\n");
    }

    else if ((!_tcsicmp(comando, _T("buy")))) {

        //Nome numAcoes
        DWORD numAcoes;
        DWORD indCarteira = getIndiceCarteira(activeUser, carteiras, numUtilizadores);
        DWORD indEmpresa;

        if (nParam == 2) {
            if (!_tcsicmp(activeUser, _T(""))) {
                return _T("Login Obrigatório.");
            }

            // O utilizador já está logado
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
            return _T("Número de parâmetros inválido.");
        }
    }

    else if ((!_tcsicmp(comando, _T("sell")))) {
        if (nParam == 2) {

            if (!_tcsicmp(activeUser, _T(""))) {
                return _T("Login Obrigatório.");
            }

            // O utilizador já está logado
            else {
                return _T("Sucesso!");

                /*
                // Alterar a lógica para venda
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
            Erro(_T("Número de parâmetros inválido."));
        }
    }
    else if ((!_tcsicmp(comando, _T("balance")))) {
        if (nParam == 0) {
            if (!_tcsicmp(activeUser, _T(""))) {
                return _T("Login Obrigatório.");
            }

            // O utilizador já está logado
            else {
                // Mostra o saldo do activeUser
                return _T("Sucesso!");
            }
        }
        else {
            Erro(_T("Número de parâmetros inválido."));
        }

    }
    else if ((!_tcsicmp(comando, _T("exit")))) {
        if (nParam == 0) {
            // Libertar os recursos desse cliente??
            return _T("EXIT");
        }
        else {
            Erro(_T("Número de parâmetros inválido."));
        }
    }
    else
        Erro(_T("Erro ao executar o comando."));
}

// Devolve o índice do utilizador
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

// Verifica se o utilizador tem a empresa X na sua carteira de Ações
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

    // Return a special value to indicate no free pipe was found
    return (DWORD)-1; // Or any other appropriate value, but not NULL
}

void inicializaPipes(HANDLE* hPipes, DWORD numPipes) {
    for (DWORD i = 0; i < numPipes; i++) {
        hPipes[i] = NULL;
    }
}

