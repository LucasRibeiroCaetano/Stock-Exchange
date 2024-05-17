#define UNICODE

#include <fcntl.h>
#include <io.h>

#include "registry.h"
#include "utils.h"

int _tmain(int argc, TCHAR* argv[]) {

    // Set Mode Verification
    DWORD stdinReturn;
    DWORD stdoutReturn;
    DWORD stderrReturn;

    // Tratamento de Ficheiros
    FILE* file;
    TCHAR line[STR_LEN];
    TCHAR fileName[STR_LEN];

    // Estruturas de Dados
    Utilizador utilizadores[MAX_USERS];
    Empresa empresas[MAX_EMPRESAS];
    UltimaTransacao ultimaTransacao;
    Eventos eventos;
    DataAdmin dataAdmin;
    CarteiraAcoes carteiras[MAX_USERS];

    // Handles
    HANDLE hPipe;

    // Dummy Values
    _tcscpy_s(ultimaTransacao.nome, STR_LEN, _T("Default"));
    ultimaTransacao.num_acoes = 0;
    ultimaTransacao.preco_acao = 0;


    // Registry
    DWORD nClientes;

    // Variáveis independentes
    DWORD numUtilizadores = 0;
    DWORD numEmpresas = 0;
    TCHAR comando[STR_LEN];
    TCHAR linhaAUX[STR_LEN];
    TCHAR linha[STR_LEN];
    DWORD nParam;
    TCHAR param[MAX_PARAM][STR_LEN];
    DWORD nSegundos = 0; // Comando Pause
    DWORD pos = 0;
    DWORD numPipes;



#ifdef UNICODE 
    stdinReturn = _setmode(_fileno(stdin), _O_WTEXT);
    stdoutReturn = _setmode(_fileno(stdout), _O_WTEXT);
    stderrReturn = _setmode(_fileno(stderr), _O_WTEXT);
#endif

    // Verificação do Set Mode
    if (stdinReturn == -1 || stdoutReturn == -1 || stderrReturn == -1)
        Abort(_T("Erro no _setmode."));

    if (argc != 2) {
        Abort(_T("Sintaxe Errada -> bolsa <nome do ficheiro de texto dos utilizadores>\n"));
    }

    // Limpar a consola
    limparConsola();

    _tprintf_s(_T("\n*********************************************************\n"));
    _tprintf_s(_T("*                                                       *\n"));
    _tprintf_s(_T("*                         Bolsa                         *\n"));
    _tprintf_s(_T("*                                                       *\n"));
    _tprintf_s(_T("*********************************************************\n"));

    if (_tcscpy_s(fileName, STR_LEN, argv[1]) != 0) {
        Abort(_T("Erro ao copiar o nome do arquivo.\n"));
    }

    if (_tfopen_s(&file, fileName, _T("r, ccs=UTF-8")) != 0 || file == NULL) {
        Abort(_T("Falha ao abrir o ficheiro.\n"));
    }

    // Ficheiro Utilizadores
    if (!_tcscmp(fileName, _T("utilizadores.txt"))) {

        while (numUtilizadores < MAX_USERS && _fgetts(line, STR_LEN, file)) {
            TCHAR usernameTemp[STR_LEN];
            TCHAR passwordTemp[STR_LEN];
            float saldoTemp;

            if (_stscanf_s(line, _T("%s %s %f"), usernameTemp, STR_LEN, passwordTemp, STR_LEN, &saldoTemp) != 3) {
                Abort(_T("Erro ao extrair username, password e/ou saldo do ficheiro.\n"));
            }

            // Copiar os valores para utilizadores[numUtilizadores]
            _tcscpy_s(utilizadores[numUtilizadores].username, STR_LEN, usernameTemp);
            _tcscpy_s(utilizadores[numUtilizadores].password, STR_LEN, passwordTemp);
            utilizadores[numUtilizadores].saldo = saldoTemp;
            utilizadores[numUtilizadores].online = false;

            // Para cada utilizador inicializar as carteiras de cada um
            _tcscpy_s(carteiras[numUtilizadores].username, STR_LEN, usernameTemp);
            carteiras[numUtilizadores].numEmpresas = 0;

            numUtilizadores++;
        }
    }

    // Ficheiro Inválido
    else
        Abort(_T("Ficheiro Inválido."));

    fclose(file);

    // A chave NCLIENTES existe, vamos ler o valor
    if (chaveExiste()) {
        MensagemInfo(_T("Chave NCLIENTES encontrada. "));
        nClientes = obterValor();

        _tprintf_s(_T("Valor NCLIENTES: %u"), nClientes);
    }
    else {
        criarChave();
        nClientes = 5;
    }

    eventos = CriarEventos();

    //----------------------------------------------- MP -----------------------------------------------
    MP mp;

    // Criação de MP
    mp.hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        SHARED_MEM_SIZE,
        TEXT("SharedMemory")); // Nome do objeto de mapeamento

    if (mp.hMapFile == NULL) {
        Abort(_T("Could not create file mapping object.\n"));
    }

    mp.pBuf = (SharedData*)MapViewOfFile(mp.hMapFile,   // Handle to map object
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        SHARED_MEM_SIZE);

    if (mp.pBuf == NULL) {
        CloseHandle(mp.hMapFile);
        Abort(_T("Could not map view of file.\n"));
    }

    mp.pBuf->numEmpresas = numEmpresas;

    // Para todas as empresas
    for (DWORD i = 0; i < numEmpresas; i++) {
        _tcscpy_s(mp.pBuf->empresas[i].nome, STR_LEN, empresas[i].nome);
        mp.pBuf->empresas[i].num_acoes = empresas[i].num_acoes;
        mp.pBuf->empresas[i].preco_acao = empresas[i].preco_acao;
    }

    // Dummy Values -> Têm de ser substituídos quando uma transação é feita. 
    // Quando um utilizador pede uma transação de compra ou venda, o servidor regista essa nesta estrutura. 
    // Vai dando overwrite e fica sempre com a última transação.
    _tcscpy_s(mp.pBuf->ultimaTransacao.nome, STR_LEN, _T("UltimaEmpresa"));
    mp.pBuf->ultimaTransacao.num_acoes = 50;
    mp.pBuf->ultimaTransacao.preco_acao = 15.75;

    //----------------------------------------------- MP -----------------------------------------------

    //----------------------------------------------- Threads ------------------------------------------
    
    numPipes = nClientes;

    // Inicializar as estruturas
    dataAdmin.empresas = empresas;
    dataAdmin.numEmpresas = numEmpresas;
    dataAdmin.eventos = eventos;
    dataAdmin.utilizadores = utilizadores;
    dataAdmin.numUtilizadores = numUtilizadores;
    dataAdmin.ultimaTransacao = ultimaTransacao;
    dataAdmin.mp = mp;
    inicializaPipes(dataAdmin.hPipes, numPipes);
    _tcscpy_s(dataAdmin.dataClientes.activeUser, STR_LEN, _T(""));

    // Thread comandos bolsa
    HANDLE hThreadComandos = CreateThread(NULL, 0, ComandosThread, &dataAdmin, 0, NULL);
    if (hThreadComandos == NULL) {
        Abort(_T("Erro ao criar a thread do administrador."));
    }

    // Preciso de criar outra thread para o board

    while (true) {

        // Criação do named pipe
        hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, 0, NULL);
        if (hPipe == INVALID_HANDLE_VALUE) {
            MensagemInfo(_T("Número máximo de clientes atingido.\n"));
        }

        if (!ConnectNamedPipe(hPipe, NULL) && GetLastError() != ERROR_PIPE_CONNECTED) {
            CloseHandle(hPipe);
            Abort(_T("ConnectNamedPipe failed.\n"));
        }

        pos = getPipe(dataAdmin.hPipes, numPipes);

        dataAdmin.dataClientes.idPipe = pos;

        dataAdmin.hPipes[pos] = hPipe; // quando se desligar faço isto = NULL

        dataAdmin.hThreads[pos] = CreateThread(NULL, 0, ClientesThread, &dataAdmin, 0, NULL);

        if (dataAdmin.hThreads[pos] == NULL) {
            CloseHandle(dataAdmin.hPipes[pos]);
            Abort(_T("CreateThread failed.\n"));
        }

        MensagemInfo(_T("Novo cliente conectado."));

    }
    //----------------------------------------------- Threads ------------------------------------------

    

    // Libertar Recursos
    MensagemInfo(_T("A libertar recursos.\n"));
    UnmapViewOfFile(mp.pBuf);
    CloseHandle(mp.hMapFile);
    CloseHandle(eventos.hRead);

    return 0;
}
