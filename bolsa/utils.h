#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>

#include "data.h"


void Abort(const TCHAR* message);
void limparConsola();
int contaParametros(const TCHAR* linha);
void extrairParametros(int numParametros, const TCHAR* linha, TCHAR comando[], TCHAR parametros[][50]);
void MensagemInfo(const TCHAR* mensagem);
void Erro(const TCHAR* mensagem);
DWORD WINAPI ComandosThread(LPVOID lpParam);
DWORD WINAPI ClientesThread(LPVOID lpParam);
TCHAR* executaComandos(TCHAR* linha, TCHAR* activeUser, DWORD numUtilizadores, DWORD numEmpresas, Empresa empresas[MAX_EMPRESAS], Utilizador utilizadores[MAX_USERS], CarteiraAcoes carteiras[MAX_USERS], UltimaTransacao ultimaTransacao);
DWORD getIndiceUtilizador(TCHAR* username, Utilizador* utilizadores, DWORD numUtilizadores);
DWORD getIndiceCarteira(TCHAR* username, CarteiraAcoes* carteiras, DWORD numUtilizadores);
DWORD getIndiceEmpresa(TCHAR* nome, Empresa* empresas, DWORD numEmpresas);
BOOL temEmpresa(TCHAR* activeUser, TCHAR* nome, CarteiraAcoes carteira);
DWORD getPipe(HANDLE* hPipes, DWORD numPipes);
void inicializaPipes(HANDLE* hPipes, DWORD numPipes);
