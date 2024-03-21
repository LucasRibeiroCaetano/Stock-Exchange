#define UNICODE

#include <fcntl.h>
#include <io.h>

#include "utils.h"


int _tmain(int argc, TCHAR* argv[]) {

    // Set Mode Verification
    DWORD stdinReturn;
    DWORD stdoutReturn;
    DWORD stderrReturn;


#ifdef UNICODE 
    stdinReturn = _setmode(_fileno(stdin), _O_WTEXT);
    stdoutReturn = _setmode(_fileno(stdout), _O_WTEXT);
    stderrReturn = _setmode(_fileno(stderr), _O_WTEXT);
#endif

    if (stdinReturn == -1 || stdoutReturn == -1 || stderrReturn == -1)
        Abort(_T("Erro no _setmode."));

    _tprintf_s(_T("\n*********************************************************\n"));
    _tprintf_s(_T("*                                                       *\n"));
    _tprintf_s(_T("*                       Cliente                         *\n"));
    _tprintf_s(_T("*                                                       *\n"));
    _tprintf_s(_T("*********************************************************\n"));










	return 0;
}