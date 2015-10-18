/*
    Arquivo com o protóripo das funções e as structs da biblioteca
*/

#define ARQ_DADOS "matches.dat"
#define IDX_PRIM "iprimary.idx"
#define IDX_WIN "iwinner.idx"
#define IDX_MVP "imvp.idx"
#define ARQ_TEMP "temp.dat"

//struct com os dados de uma partida
typedef struct{
    char codigo[9];
    char equipeVermelha[40];
    char equipeAzul[40];
    char data[11];
    char duracao[6];
    char equipeVencedora[40];
    char placarAzul[3];
    char placarVermelho[3];
    char mvp[40];
}Partida;

//struct com os dados do índice primário
typedef struct{
    char codigo[9];
    long rrn;
}RegistroPrim;

//struct com os dados do índice secundário de equipe vencedora
typedef struct{
    char equipeVencedora[40];
    char codigo[9];
}RegistroWin;

//struct com os dados do índice secundário de mvp
typedef struct{
    char mvp[40];
    char codigo[9];
}RegistroMvp;

//protótipo das funções
int comparePrim(const void * a, const void * b);
int compareWin(const void * a, const void * b);
int compareMvp(const void * a, const void * b);
long gerarIdx(RegistroPrim **idxPrim, RegistroWin **idxWin, RegistroMvp **idxMvp);
long inicializarIdx(RegistroPrim **idxPrim, RegistroWin **idxWin, RegistroMvp **idxMvp);
void meuGet(char string[], char erro[], int limite);
int validaData(const char string[]);
int validaDuracao(const char string[]);
int validaPlacar(const char string[]);
void gerarCod(Partida *p);
void criarRegistro(Partida *p, char registro[]);
int verificarChave(RegistroPrim **idxPrim, char chave[], long count);
long inserirPartida(RegistroPrim **idxPrim, RegistroWin **idxWin, RegistroMvp **idxMvp, long count);
long buscaBin(RegistroPrim **idxPrim, char chave[], long min, long max);
void alterarDuracao(RegistroPrim **idxPrim, RegistroPrim *chave, long count);
void removerPartida(RegistroPrim **idxPrim, RegistroPrim *chave, long count);
void buscarPrim(RegistroPrim **idxPrim, RegistroPrim *chave, long count);
void buscarWin(RegistroPrim **idxPrim, RegistroWin **idxWin, char equipe[], long count);
void buscarMvp(RegistroPrim **idxPrim, RegistroMvp **idxMvp, char mvp[], long count);
void listarPrim(RegistroPrim **idxPrim, long count);
void listarWin(RegistroPrim **idxPrim, RegistroWin **idxWin, long count);
void listarMvp(RegistroPrim **idxPrim, RegistroMvp **idxMvp, long count);
long liberar(RegistroPrim **idxPrim, RegistroWin **idxWin, RegistroMvp **idxMvp, long count);
void gravarIdx(RegistroPrim **idxPrim, RegistroWin **idxWin, RegistroMvp **idxMvp, long count);
