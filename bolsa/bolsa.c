#define UNICODE

#include <fcntl.h>
#include <io.h>

#include "data.h"
#include "utils.h"

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

    // Variáveis independentes
    DWORD numUtilizadores = 0;
    TCHAR comando[STR_LEN];
    TCHAR linha[STR_LEN];
    DWORD nParam;
    TCHAR param[MAX_PARAM][STR_LEN];


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
                Abort(_T("Erro ao extrair username, password e/ou saldo do ficheiro."));
            }

            // Copiar os valores para utilizadores[numUtilizadores]
            _tcscpy_s(utilizadores[numUtilizadores].username, STR_LEN, usernameTemp);
            _tcscpy_s(utilizadores[numUtilizadores].password, STR_LEN, passwordTemp);
            utilizadores[numUtilizadores].saldo = saldoTemp;

            numUtilizadores++;
        }
    }

    // Ficheiro Inválido
    else
        Abort(_T("Ficheiro Inválido."));

    fclose(file);

    _tprintf(_T("\n[ INFO ] Informação Lida do ficheiro de texto:\n\n"));

    _tprintf_s(_T("*********************************************************\n\n"));

    for (DWORD i = 0; i < numUtilizadores; i++) {
        _tprintf(_T("Utilizador %d:\n"), i + 1);
        _tprintf(_T("  - Username: %s\n"), utilizadores[i].username);
        _tprintf(_T("  - Password: %s\n"), utilizadores[i].password);
        _tprintf(_T("  - Saldo:    %.2f €\n\n"), utilizadores[i].saldo);
    }

    _tprintf_s(_T("*********************************************************\n\n"));


    // Vamos ler a registry e ver se existe ua chave NCLIENTES, se existir usar o valor 

    // Se nao existir criamos uma key NCLIENTES com valor 5

    // Interface de gestão de comandos
    _tprintf(_T("Escreva 'ajuda' para uma lista completa de comandos.\n\n"));

    while (1) {
        _tprintf(_T("Administrador> "));

        _fgetts(linha, STR_LEN, stdin);

        // Obtém o tamanho do comando
        size_t length = _tcslen(linha);

        // Se o comando não estiver vazio truncar o \n
        if (length > 0 && linha[length - 1] == '\n') {
            linha[length - 1] = '\0';
        }

        nParam = contaParametros(linha);

        // Repartir o comando e os parâmetros
        extrairParametros(nParam, linha, comando, param);

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

        // Adicionar empresas por ficheiro
        else if (!_tcsicmp(comando, _T("addf"))) {
            if (nParam == 1) {

                TCHAR nomeFich[STR_LEN];
                TCHAR linhaFich[STR_LEN];
                DWORD numEmpresas = 0;
                _tcscpy_s(nomeFich, STR_LEN, param[1]);

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

                // Imprimir as empresas lidas
                _tprintf(_T("Empresas lidas do arquivo:\n"));
                for (int i = 0; i < numEmpresas; i++) {
                    _tprintf(_T("Empresa %d:\n"), i + 1);
                    _tprintf(_T("  - Nome: %s\n"), empresas[i].nome);
                    _tprintf(_T("  - Número de ações: %d\n"), empresas[i].num_acoes);
                    _tprintf(_T("  - Preço da ação: %.2f\n"), empresas[i].preco_acao);
                }

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


    return 0;
}
