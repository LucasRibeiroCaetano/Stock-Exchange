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

    // Ficheiro Empresas
    else if (!_tcscmp(fileName, _T("empresas.txt"))) {


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

        // Recebe o comando do administrador
        _tscanf_s(_T("%s"), comando, STR_LEN);

        // Sair do programa
        if (_tcsicmp(comando, _T("close")) == 0) {
            _tprintf(_T("Encerrando o programa...\n"));
            return 0;
        }
    }


    return 0;
}
