/*#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

#define REG_PATH "Software\\NomeDaSuaEmpresa\\NomeDoSeuPrograma"
#define REG_KEY "NCLIENTES"

int _tmain(int argc, TCHAR* argv[]) {
    HKEY hKey;
    DWORD dwDisposition;
    DWORD dwType = REG_DWORD;
    DWORD dwData;
    DWORD dwSize = sizeof(DWORD);
    DWORD dwDefaultValue = 5;

    // Try to open the registry key
    LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T(REG_PATH), 0, KEY_READ | KEY_WRITE, &hKey);

    if (lResult == ERROR_SUCCESS) {
        // Key exists, so read the value
        lResult = RegQueryValueEx(hKey, _T(REG_KEY), NULL, &dwType, (LPBYTE)&dwData, &dwSize);
        if (lResult == ERROR_SUCCESS) {
            // Key exists and value read successfully
            _tprintf(_T("Value of %s is: %u\n"), _T(REG_KEY), dwData);
        }
        else {
            // Key exists but failed to read value, so set a default value
            _tprintf(_T("Failed to read %s. Using default value: %u\n"), _T(REG_KEY), dwDefaultValue);
            dwData = dwDefaultValue;
        }
    }
    else {
        // Key doesn't exist, so create it with default value
        _tprintf(_T("%s key does not exist. Creating it with default value: %u\n"), _T(REG_KEY), dwDefaultValue);
        lResult = RegCreateKeyEx(HKEY_CURRENT_USER, _T(REG_PATH), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);
        if (lResult == ERROR_SUCCESS) {
            // Set the default value
            lResult = RegSetValueEx(hKey, _T(REG_KEY), 0, REG_DWORD, (const BYTE*)&dwDefaultValue, sizeof(DWORD));
            if (lResult != ERROR_SUCCESS) {
                Abort(_T("Failed to set value for %s during creation.\n"), REG_KEY);
            }
        }
        else {
            Abort(_T("Failed to create %s key.\n"), REG_KEY);
        }
    }

    // Close the registry key
    RegCloseKey(hKey);

    return 0;
}
*/