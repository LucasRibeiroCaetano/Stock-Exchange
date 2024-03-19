#define UNICODE

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "data.h"

int _tmain(int argc, TCHAR* argv[]) {

    TCHAR fileName[STR_LEN];
    Utilizador utilizadores[MAX_USERS];
    DWORD numUtilizadores = 0;


#ifdef UNICODE 
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    if (argc != 2) {
        Abort(_T("Sintaxe Errada -> bolsa <nome do ficheiro de texto dos utilizadores>\n"));
    }

    _tprintf_s(_T("=|| Bolsa ||=\n"));
    _tprintf_s(_T("A ler do ficheiro de texto..."));

    // Copia o nome do ficheiro dos argumentos
    if (_tcscpy_s(fileName, STR_LEN, argv[1]) != 0) {
        Abort(_T("Erro ao copiar o nome do arquivo.\n"));
    }

    // Abrir o ficheiro
    FILE* file;
    if (_tfopen_s(&file, fileName, _T("r")) != 0 || file == NULL) {
        Abort(_T("Falha ao abrir o arquivo de utilizadores.\n"));
    }

    // Ler o conteúdo do ficheiro e armazenar nos utilizadores
    while (numUtilizadores < MAX_USERS) {

        // Username
        if (_fgetts(utilizadores[numUtilizadores].username, STR_LEN, fileName) == NULL)
            Abort("Ficheiro de utilizadores -> Utilizador Inválido");

        // Password
        if (_fgetts(utilizadores[numUtilizadores].password, STR_LEN, fileName) == NULL)
            Abort("Ficheiro de utilizadores -> Password Inválido");

        // Saldo
        if (_ftscanf_s(fileName, _T("%f"), &utilizadores[numUtilizadores].saldo) != 1)
            Abort("Ficheiro de utilizadores -> Saldo Inválido");

        numUtilizadores++;
    }

    fclose(file);

    // Mostrar os utilizadores
    for (DWORD i = 0; i < numUtilizadores; i++) {
        _tprintf(_T("Utilizador %d:\n"), i + 1);
        _tprintf(_T("Username: %s\n"), utilizadores[i].username);
        _tprintf(_T("Password: %s\n"), utilizadores[i].password);
        _tprintf(_T("Saldo: %.2f\n"), utilizadores[i].saldo);
    }

    // Entrar na registry ver se existe um valor NCLIENTES, se não existir NCLIENTES = 5 na registry

    return 0;
}