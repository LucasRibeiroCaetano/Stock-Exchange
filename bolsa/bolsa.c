#define UNICODE

#include <fcntl.h>
#include <io.h>

#include "data.h"
#include "utils.h"

#include "registry.h"
#include "eventos.h"

int _tmain(int argc, TCHAR* argv[]) {

    // Set Mode Verification
    DWORD stdinReturn;
    DWORD stdoutReturn;
    DWORD stderrReturn;

    // Tratamento de Ficheiros
    FILE* file;
    TCHAR line[STR_LEN];
    TCHAR fileName[STR_LEN];

    // Estruturas de Dados
    Utilizador utilizadores[MAX_USERS];
    Empresa empresas[MAX_EMPRESAS];

    // Registry
    DWORD nClientes;

    // Variáveis independentes
    DWORD numUtilizadores = 0;
    DWORD numEmpresas = 0;
    TCHAR comando[STR_LEN];
    TCHAR linhaAUX[STR_LEN];
    TCHAR linha[STR_LEN];
    DWORD nParam;
    TCHAR param[MAX_PARAM][STR_LEN];
    DWORD nSegundos = 0; // Comando Pause


#ifdef UNICODE 
    stdinReturn = _setmode(_fileno(stdin), _O_WTEXT);
    stdoutReturn = _setmode(_fileno(stdout), _O_WTEXT);
    stderrReturn = _setmode(_fileno(stderr), _O_WTEXT);
#endif

    // Verificação do Set Mode
    if (stdinReturn == -1 || stdoutReturn == -1 || stderrReturn == -1)
        Abort(_T("Erro no _setmode."));

    if (argc != 2) {
        Abort(_T("Sintaxe Errada -> bolsa <nome do ficheiro de texto dos utilizadores>\n"));
    }

    // Limpar a consola
    limparConsola();

    _tprintf_s(_T("\n*********************************************************\n"));
    _tprintf_s(_T("*                                                       *\n"));
    _tprintf_s(_T("*                         Bolsa                         *\n"));
    _tprintf_s(_T("*                                                       *\n"));
    _tprintf_s(_T("*********************************************************\n"));

    if (_tcscpy_s(fileName, STR_LEN, argv[1]) != 0) {
        Abort(_T("Erro ao copiar o nome do arquivo.\n"));
    }

    if (_tfopen_s(&file, fileName, _T("r")) != 0 || file == NULL) {
        Abort(_T("Falha ao abrir o ficheiro.\n"));
    }

    // Ficheiro Utilizadores
    if (!_tcscmp(fileName, _T("utilizadores.txt"))) {

        while (numUtilizadores < MAX_USERS && _fgetts(line, STR_LEN, file)) {
            TCHAR usernameTemp[STR_LEN];
            TCHAR passwordTemp[STR_LEN];
            float saldoTemp;

            if (_stscanf_s(line, _T("%s %s %f"), usernameTemp, STR_LEN, passwordTemp, STR_LEN, &saldoTemp) != 3) {
                Abort(_T("Erro ao extrair username, password e/ou saldo do ficheiro.\n"));
            }

            // Copiar os valores para utilizadores[numUtilizadores]
            _tcscpy_s(utilizadores[numUtilizadores].username, STR_LEN, usernameTemp);
            _tcscpy_s(utilizadores[numUtilizadores].password, STR_LEN, passwordTemp);
            utilizadores[numUtilizadores].saldo = saldoTemp;
            utilizadores[numUtilizadores].online = false;

            numUtilizadores++;
        }
    }

    // Ficheiro Inválido
    else
        Abort(_T("Ficheiro Inválido."));

    fclose(file);

    // A chave NCLIENTES existe, vamos ler o valor
    if (chaveExiste()) {
        MensagemInfo(_T("Chave NCLIENTES encontrada. "));
        nClientes = obterValor();

        _tprintf_s(_T("Valor NCLIENTES: %u\n\n"), nClientes);
    }
    else {
        criarChave();
        nClientes = 5;
    }

    //----------------------------------------------- MP -----------------------------------------------

    HANDLE hMapFile; // Handle da MP
    SharedData* pBuf; // Estrutura de dados onde é escrita a informação na MP

    // Criação de MP
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        SHARED_MEM_SIZE,
        TEXT("SharedMemory")); // Nome do objeto de mapeamento

    if (hMapFile == NULL) {
        _tprintf_s(_T("Could not create file mapping object (%d).\n"), GetLastError());
        return 1;
    }

    pBuf = (SharedData*)MapViewOfFile(hMapFile,   // Handle to map object
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        SHARED_MEM_SIZE);

    if (pBuf == NULL) {
        _tprintf_s(_T("Could not map view of file (%d).\n"), GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    pBuf->numEmpresas = numEmpresas;

    // Para todas as empresas
    for (DWORD i = 0; i < numEmpresas; i++) {
        _tcscpy_s(pBuf->empresas[i].nome, STR_LEN, empresas[i].nome);
        pBuf->empresas[i].num_acoes = empresas[i].num_acoes;
        pBuf->empresas[i].preco_acao = empresas[i].preco_acao;
    }


    // Dummy Values -> Têm de ser substituídos quando uma transação é feita. 
    // Quando um utilizador pede uma transação de compra ou venda, o servidor regista essa nesta estrutura. 
    // Vai dando overwrite e fica sempre com a última transação.
    _tcscpy_s(pBuf->ultimaTransacao.nome, STR_LEN, _T("UltimaEmpresa"));
    pBuf->ultimaTransacao.num_acoes = 50;
    pBuf->ultimaTransacao.preco_acao = 15.75;
















































    //----------------------------------------------- MP -----------------------------------------------

    // Interface de gestão de comandos
    _tprintf(_T("Escreva 'ajuda' para uma lista completa de comandos.\n\n"));

    while (1) {
        _tprintf(_T("Administrador> "));

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
                    _tprintf(_T("Erro ao extrair informações da empresa do arquivo.\n"));
                    fclose(file);
                    return -1;
                }

                // Armazenar informações na estrutura
                _tcscpy_s(empresas[numEmpresas].nome, STR_LEN, nome);
                empresas[numEmpresas].num_acoes = num_acoes;
                empresas[numEmpresas].preco_acao = preco_acao;

                numEmpresas++;
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
                if (_tfopen_s(&file, nomeFich, _T("r")) != 0 || file == NULL) {
                    _tprintf(_T("Falha ao abrir o arquivo.\n"));
                    return -1;
                }

                // Ler as empresas do ficheiro
                while (numEmpresas < MAX_EMPRESAS && _fgetts(linhaFich, STR_LEN, file)) {
                    TCHAR nome[STR_LEN];
                    int num_acoes;
                    float preco_acao;

                    // Extrair informações da linha
                    if (_stscanf_s(linhaFich, _T("%s %d %f"), nome, STR_LEN, &num_acoes, &preco_acao) != 3) {
                        _tprintf(_T("Erro ao extrair informações da empresa do arquivo.\n"));
                        fclose(file);
                        return -1;
                    }

                    // Copiar as informações para a estrutura de dados
                    _tcscpy_s(empresas[numEmpresas].nome, STR_LEN, nome);
                    empresas[numEmpresas].num_acoes = num_acoes;
                    empresas[numEmpresas].preco_acao = preco_acao;

                    numEmpresas++;
                }

                fclose(file);
            }
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));

        }

        // Listar todas as empresas
        else if (!_tcsicmp(comando, _T("listc"))) {
            if (nParam == 0) {
                
                if (numEmpresas == 0)
                    _tprintf_s(_T("\nNão existem empresas.\n\n"));

                else {
                    _tprintf_s(_T("\n*********************************************************\n"));
                    _tprintf(_T("\nEmpresas lidas do arquivo:\n\n"));

                    for (int i = 0; i < numEmpresas; i++) {
                        _tprintf(_T("Empresa %d:\n"), i + 1);
                        _tprintf(_T("  - Nome: %s\n"), empresas[i].nome);
                        _tprintf(_T("  - Número de ações: %u\n"), empresas[i].num_acoes);
                        _tprintf(_T("  - Preço da ação: %.2f\n\n"), empresas[i].preco_acao);
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

                for (int i = 0; i < numEmpresas; i++) {
                    if (!_tcscmp(empresas[i].nome, param[0])) {
                        if (_stscanf_s(param[1], _T("%f"), &empresas[i].preco_acao) != 1) {
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

                for (DWORD i = 0; i < numUtilizadores; i++) {
                    _tprintf(_T("%s %s:\n"), utilizadores[i].online ? _T("\033[32m\u25CF\033[0m") : _T("\033[31m\u25CF\033[0m"), utilizadores[i].username);
                    _tprintf(_T("    Saldo:    %.2f €\n\n"), utilizadores[i].saldo);
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

            if(nParam == 0)
                limparConsola();
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));
        }
            
        // Sair do programa
        else if (_tcsicmp(comando, _T("close")) == 0) {

            if (nParam == 0) {
                _tprintf(_T("\nEncerrando o programa...\n\n"));
                return 0;
            }
            else
                _tprintf(_T("\nNúmero de parâmetros inválido.\n"));
        }

        else {
            _tprintf(_T("\nComando não reconhecido... Escreva 'ajuda' para uma lista completa de comandos. \n\n"));
        }
    }

    // Libertar os recursos da memória partilhada
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}
