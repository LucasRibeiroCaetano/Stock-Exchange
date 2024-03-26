#define UNICODE

#include <fcntl.h>
#include <io.h>

#include "data.h"
#include "utils.h"

#include "registry.h"

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

    // Vari�veis independentes
    DWORD numUtilizadores = 0;
    TCHAR comando[STR_LEN];
    TCHAR linhaAUX[STR_LEN];
    TCHAR linha[STR_LEN];
    DWORD nParam;
    TCHAR param[MAX_PARAM][STR_LEN];
    DWORD numEmpresas = 0;


#ifdef UNICODE 
    stdinReturn = _setmode(_fileno(stdin), _O_WTEXT);
    stdoutReturn = _setmode(_fileno(stdout), _O_WTEXT);
    stderrReturn = _setmode(_fileno(stderr), _O_WTEXT);
#endif

    // Verifica��o do Set Mode
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

            numUtilizadores++;
        }
    }

    // Ficheiro Inv�lido
    else
        Abort(_T("Ficheiro Inv�lido."));

    fclose(file);

    _tprintf(_T("\n[ INFO ] Informa��o Lida do ficheiro de texto:\n\n"));

    _tprintf_s(_T("*********************************************************\n\n"));

    for (DWORD i = 0; i < numUtilizadores; i++) {
        _tprintf(_T("Utilizador %d:\n"), i + 1);
        _tprintf(_T("  - Username: %s\n"), utilizadores[i].username);
        _tprintf(_T("  - Password: %s\n"), utilizadores[i].password);
        _tprintf(_T("  - Saldo:    %.2f �\n\n"), utilizadores[i].saldo);
    }

    _tprintf_s(_T("*********************************************************\n"));

    // A chave NCLIENTES existe, vamos ler o valor
    if (chaveExiste()) {
        _tprintf_s(_T("\nChave NCLIENTES encontrada.\n"));
        nClientes = obterValor();

        _tprintf_s(_T("Valor NCLIENTES: %u\n\n"), nClientes);
    }
    else {
        criarChave();
        nClientes = 5;
    }

    // Interface de gest�o de comandos
    _tprintf(_T("Escreva 'ajuda' para uma lista completa de comandos.\n\n"));

    while (1) {
        _tprintf(_T("Administrador> "));

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
                    _tprintf(_T("Erro ao extrair informa��es da empresa do arquivo.\n"));
                    fclose(file);
                    return -1;
                }

                // Armazenar informa��es na estrutura
                _tcscpy_s(empresas[numEmpresas].nome, STR_LEN, nome);
                empresas[numEmpresas].num_acoes = num_acoes;
                empresas[numEmpresas].preco_acao = preco_acao;

                numEmpresas++;
            }
            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));

        }

        // Listar todas as empresas
        else if (!_tcsicmp(comando, _T("listc"))) {
            if (nParam == 0) {
                // Imprimir as empresas lidas
                _tprintf_s(_T("\n*********************************************************\n"));
                _tprintf(_T("\nEmpresas lidas do arquivo:\n\n"));
                for (int i = 0; i < numEmpresas; i++) {
                    _tprintf(_T("Empresa %d:\n"), i + 1);
                    _tprintf(_T("  - Nome: %s\n"), empresas[i].nome);
                    _tprintf(_T("  - N�mero de a��es: %u\n"), empresas[i].num_acoes);
                    _tprintf(_T("  - Pre�o da a��o: %.2f\n\n"), empresas[i].preco_acao);
                }
                _tprintf_s(_T("*********************************************************\n\n"));
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
                if (_tfopen_s(&file, nomeFich, _T("r")) != 0 || file == NULL) {
                    _tprintf(_T("Falha ao abrir o arquivo.\n"));
                    return -1;
                }

                // Ler as empresas do ficheiro
                while (numEmpresas < MAX_EMPRESAS && _fgetts(linhaFich, STR_LEN, file)) {
                    TCHAR nome[STR_LEN];
                    int num_acoes;
                    float preco_acao;

                    // Extrair informa��es da linha
                    if (_stscanf_s(linhaFich, _T("%s %d %f"), nome, STR_LEN, &num_acoes, &preco_acao) != 3) {
                        _tprintf(_T("Erro ao extrair informa��es da empresa do arquivo.\n"));
                        fclose(file);
                        return -1;
                    }

                    // Copiar as informa��es para a estrutura de dados
                    _tcscpy_s(empresas[numEmpresas].nome, STR_LEN, nome);
                    empresas[numEmpresas].num_acoes = num_acoes;
                    empresas[numEmpresas].preco_acao = preco_acao;

                    numEmpresas++;
                }

                fclose(file);
            }
            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));

        }

        // Limpar a Consola
        else if (!_tcsicmp(comando, _T("limpar"))) {

            if(nParam == 0)
                limparConsola();
            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));
        }
            
        // Sair do programa
        else if (_tcsicmp(comando, _T("close")) == 0) {

            if (nParam == 0) {
                _tprintf(_T("\nEncerrando o programa...\n\n"));
                return 0;
            }
            else
                _tprintf(_T("\nN�mero de par�metros inv�lido.\n"));
        }

        else {
            _tprintf(_T("\nComando n�o reconhecido... Escreva 'ajuda' para uma lista completa de comandos. \n\n"));
        }
    }


    return 0;
}
