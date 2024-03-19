#include "utils.h"



void Abort(const TCHAR* message) {
    _ftprintf(stderr, _T("\033[1;31mErro: %s\033[0m\n"), message);
    exit(EXIT_FAILURE);
}