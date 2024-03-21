#include "utils.h"


void Abort(const TCHAR* message) {
    DWORD errorCode = GetLastError(); // Retrieve the error code
    LPVOID lpMsgBuf; // Buffer to hold error message
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
    );

    // Print error message along with the custom message
    _ftprintf(stderr, _T("\n\033[1;31m%s\033[0m"), message);
    _ftprintf(stderr, _T("\033[1;31mDetalhes do erro: %s\033[0m\n"), (LPCTSTR)lpMsgBuf);

    // Free the buffer
    LocalFree(lpMsgBuf);

    exit(EXIT_FAILURE);
}
