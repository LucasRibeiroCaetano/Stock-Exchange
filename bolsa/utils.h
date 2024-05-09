#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>


void Abort(const TCHAR* message);
void limparConsola();
int contaParametros(const TCHAR* linha);
void extrairParametros(int numParametros, const TCHAR* linha, TCHAR comando[], TCHAR parametros[][50]);
void MensagemInfo(const TCHAR* mensagem);
//void libertarRecursos(MP mp, Eventos eventos);