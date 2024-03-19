#define UNICODE

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <fcntl.h>
#include <io.h>

#include "data.h"

#define STR_LEN 100 // Assuming maximum length of strings

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
    _tprintf_s(_T("A ler do ficheiro de texto...\n"));

    if (_tcscpy_s(fileName, STR_LEN, argv[1]) != 0) {
        Abort(_T("Erro ao copiar o nome do arquivo.\n"));
    }

    _tprintf_s(_T("\n\nNome do ficheiro: %s\n"), fileName);

    FILE* file;
    if (_tfopen_s(&file, fileName, _T("r")) != 0 || file == NULL) {
        Abort(_T("Falha ao abrir o arquivo de utilizadores.\n"));
    }

    while (numUtilizadores < MAX_USERS && !feof(file)) {
        if (_fgetts(utilizadores[numUtilizadores].username, STR_LEN, file) == NULL)
            Abort("Erro ao ler os usernames dos utilizadores.");

        if (_fgetts(utilizadores[numUtilizadores].password, STR_LEN, file) == NULL)
            Abort("Erro ao ler as passwords dos utilizadores.");

        if (_ftscanf_s(file, _T("%f"), &utilizadores[numUtilizadores].saldo) != 1)
            Abort("Erro ao ler o saldo dos utilizadores.");

        numUtilizadores++;
    }


    fclose(file);

    _tprintf_s(_T("\nChegou aqui. numUtil: %u\n"), numUtilizadores);

    for (DWORD i = 0; i < numUtilizadores; i++) {
        _tprintf(_T("Utilizador %d:\n"), i + 1);
        _tprintf(_T("Username: %s\n"), utilizadores[i].username);
        _tprintf(_T("Password: %s\n"), utilizadores[i].password);
        _tprintf(_T("Saldo: %.2f\n"), utilizadores[i].saldo);
    }

    return 0;
}
