#define UNICODE

#include <fcntl.h>
#include <io.h>

#include "data.h"

int _tmain(int argc, TCHAR* argv[]) {

    // Set Mode Verification
    DWORD stdinReturn;
    DWORD stdoutReturn;
    DWORD stderrReturn;

    // Verificação do Set Mode
    if (stdinReturn == -1 || stdoutReturn == -1 || stderrReturn == -1)
        Abort(_T("Erro no _setmode."));

    if (argc != 2) {
        Abort(_T("Sintaxe Errada -> board <número de empresas>\n"));
    }








#ifdef UNICODE 
stdinReturn = _setmode(_fileno(stdin), _O_WTEXT);
stdoutReturn = _setmode(_fileno(stdout), _O_WTEXT);
stderrReturn = _setmode(_fileno(stderr), _O_WTEXT);
#endif