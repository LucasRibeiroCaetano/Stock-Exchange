#include "registry.h"
#include "utils.h"

#define KEY_PATH _T("Software\\SO2\\NCLIENTES")
#define KEY_VALUE _T("NCLIENTES")

int chaveExiste() {
    HKEY hKey;
    LONG resultado = RegOpenKeyEx(HKEY_CURRENT_USER, KEY_PATH, 0, KEY_READ, &hKey);

    // Se a diretoria principal (SO2) nao existir
    if (resultado != ERROR_SUCCESS) {
        return 0;
    }

    RegCloseKey(hKey);
    return 1;
}

void criarChave() {
    HKEY hKey;
    DWORD dwData = 5;
    LONG resultado = RegCreateKeyEx(HKEY_CURRENT_USER, KEY_PATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if (resultado != ERROR_SUCCESS) {
        Abort(_T("Erro ao criar a chave da registry.\n"));
    }
    else {
        resultado = RegSetValueEx(hKey, KEY_VALUE, 0, REG_DWORD, (BYTE*)&dwData, sizeof(DWORD));
        if (resultado == ERROR_SUCCESS) {
            _tprintf_s(_T("\n[ INFO ] Chave %s criada com sucesso e valor definido como 5.\n\n"), KEY_PATH);
        }
        else {
            _tprintf_s(_T("\n[ INFO ] Chave %s criada, mas houve um erro ao definir o valor.\n"), KEY_PATH);
        }
        RegCloseKey(hKey);
    }
}


DWORD obterValor() {
    HKEY hKey;
    DWORD dwValor, dwTipo, dwTamanho;
    LONG resultado = RegOpenKeyEx(HKEY_CURRENT_USER, KEY_PATH, 0, KEY_READ, &hKey);
    if (resultado != ERROR_SUCCESS) {
        Abort(_T("Erro ao abrir a chave da registry.\n"));
        return 0;
    }
    dwTamanho = sizeof(DWORD);
    resultado = RegQueryValueEx(hKey, KEY_VALUE, NULL, &dwTipo, (LPBYTE)&dwValor, &dwTamanho);
    if (resultado != ERROR_SUCCESS || dwTipo != REG_DWORD) {
        Abort(_T("Erro ao obter o valor da chave da registry.\n"));
        RegCloseKey(hKey);
        return 0;
    }

    RegCloseKey(hKey);
    return dwValor;
}
