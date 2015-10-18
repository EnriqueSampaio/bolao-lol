/*
    Arquivo com a implementação das funções da biblioteca
*/
#include "380229_587320_ED2_T01.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//Função de comparação de registros do índice primário, para auxílio da função quicksort
int comparePrim(const void * a, const void * b){
    RegistroPrim *elem1 = (RegistroPrim*)a;
    RegistroPrim *elem2 = (RegistroPrim*)b;

    return strcmp(elem1->codigo, elem2->codigo);
}

//Função de comparação de registros do índice secundário de equipe vencedora, para auxílio da função quicksort
int compareWin(const void * a, const void * b){
    RegistroWin *elem1 = (RegistroWin*)a;
    RegistroWin *elem2 = (RegistroWin*)b;
    int cmp;

    cmp = strcmp(elem1->equipeVencedora, elem2->equipeVencedora);
    if(cmp == 0){
        return strcmp(elem1->codigo, elem2->codigo);
    } else{
        return cmp;
    }
}

//Função de comparação de registros do índice secundário de mvp, para auxílio da função quicksort
int compareMvp(const void * a, const void * b){
    RegistroMvp *elem1 = (RegistroMvp*)a;
    RegistroMvp *elem2 = (RegistroMvp*)b;
    int cmp;

    cmp = strcmp(elem1->mvp, elem2->mvp);
    if(cmp == 0){
        return strcmp(elem1->codigo, elem2->codigo);
    } else{
        return cmp;
    }
}

//função para gerar os índices, caso não existam ou estejam inconsistentes
long gerarIdx(RegistroPrim **idxPrim, RegistroWin **idxWin, RegistroMvp **idxMvp){
    FILE *iprimary, *iwinner, *imvp, *matches;
    char lixo[40];
    long i, count = 1;
    size_t arqSize;

    //Verifica se o arquivo de dados existe
    if((matches = fopen(ARQ_DADOS, "r+")) == NULL){
        matches = fopen(ARQ_DADOS, "w+");
    }

    //identificando o tamanho do arquivo
    fseek(matches, 0, SEEK_END);
    arqSize = ftell(matches);
    fseek(matches, 0, SEEK_SET);
    //índice primário
    iprimary = fopen(IDX_PRIM, "w+");
    //índice vencedor
    iwinner = fopen(IDX_WIN, "w+");
    //indice mvp
    imvp = fopen(IDX_MVP, "w+");
    fprintf(iprimary, "1\n");
    while (ftell(matches) < arqSize) {
        (*idxPrim) = realloc((*idxPrim), count * sizeof(RegistroPrim));
        (*idxWin) = realloc((*idxWin), count * sizeof(RegistroWin));
        (*idxMvp) = realloc((*idxMvp), count * sizeof(RegistroMvp));
        fscanf(matches, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", (*idxPrim)[count-1].codigo, lixo, lixo, lixo, lixo, (*idxWin)[count-1].equipeVencedora, lixo, lixo, (*idxMvp)[count-1].mvp);
        (*idxPrim)[count-1].rrn = (count - 1) * 192;
        strcpy((*idxWin)[count-1].codigo, (*idxPrim)[count-1].codigo);
        strcpy((*idxMvp)[count-1].codigo, (*idxPrim)[count-1].codigo);
        fseek(matches, count * 192, SEEK_SET);
        count++;
    }
    count--;
    qsort((*idxPrim), count, sizeof(RegistroPrim), comparePrim);
    qsort((*idxWin), count, sizeof(RegistroWin), compareWin);
    qsort((*idxMvp), count, sizeof(RegistroMvp), compareMvp);
    for (i = 0; i < count; i++) {
        fprintf(iprimary, "%s %ld\n", (*idxPrim)[i].codigo, (*idxPrim)[i].rrn);
        fprintf(iwinner, "%s@%s\n", (*idxWin)[i].equipeVencedora, (*idxWin)[i].codigo);
        fprintf(imvp, "%s@%s\n", (*idxMvp)[i].mvp, (*idxMvp)[i].codigo);
    }

    fclose(matches);
    fclose(iprimary);
    fclose(iwinner);
    fclose(imvp);
    return (count);
}

//função que inicializa os índices ao abrir o programa, ou então chama a função gravarIdx caso não existam ou estejam inconsistentes
long inicializarIdx(RegistroPrim **idxPrim, RegistroWin **idxWin, RegistroMvp **idxMvp){
    FILE *iprimary, *iwinner, *imvp;
    char chave[9];
    int flag;
    long rrn, count = 1;

    //Verifica se os índices existem
    if((iprimary = fopen(IDX_PRIM, "r+")) != NULL && (iwinner = fopen(IDX_WIN, "r+")) != NULL && (imvp = fopen(IDX_MVP, "r+")) != NULL){
        /*
        Recebe o bit verificador do índice primário, e se o mesmo for 1,
        o índice não foi atualizado corretamente na última execução do programa,
        caso seja 0, ele está correto
        */
        fscanf(iprimary, "%d\n", &flag);
        if(flag){
            fclose(iwinner);
            fclose(imvp);
            fclose(iprimary);
            count = gerarIdx(idxPrim, idxWin, idxMvp);
        } else{
            fseek(iprimary, 0, SEEK_SET);
            fprintf(iprimary, "1\n");
            while(fscanf(iprimary, "%s %ld\n", chave, &rrn) != EOF){
                (*idxPrim) = realloc((*idxPrim), count * sizeof(RegistroPrim));
                (*idxWin) = realloc((*idxWin), count * sizeof(RegistroWin));
                (*idxMvp) = realloc((*idxMvp), count * sizeof(RegistroMvp));
                fscanf(iwinner, "%[^@]%*c%[^\n]%*c", (*idxWin)[count-1].equipeVencedora, (*idxWin)[count-1].codigo);
                fscanf(imvp, "%[^@]%*c%[^\n]%*c", (*idxMvp)[count-1].mvp, (*idxMvp)[count-1].codigo);
                strcpy((*idxPrim)[count-1].codigo, chave);
                (*idxPrim)[count-1].rrn = rrn;
                count++;
            }
            count--;
            fclose(iwinner);
            fclose(imvp);
            fclose(iprimary);
        }
    } else{
        count = gerarIdx(idxPrim, idxWin, idxMvp);
    }

    return (count);
}

//função customizada para receber strings até encontrar um \n, e também recebe o número limite de bytes que o registro pode ter e a mensagem de erro que deve ser exibida, caso o limite seja excedido
void meuGet(char string[], char erro[], int limite){
    char c;
    int i, flag = 0;

    do {
        if (flag) {
            printf("%s", erro);
        }
        flag = 0;
        scanf("%c", &c);
        for (i = 0; c != '\n'; i++) {
            string[i] = c;
            scanf("%c", &c);
            if(i >= limite){
                flag = 1;
            }
        }
    } while(flag);
    string[i] = '\0';
}

//função para validar se a data está no formato DD/MM/YYYY onde o dia está entre 01 e 31, o mês entre 01 e 12 e o ano entre 2011 e 2015
int validaData(const char string[]){
    char *dia, *mes, *ano, *data;
    long diaNum, mesNum, anoNum;

    data = malloc(sizeof(string));
    strcpy(data, string);

    dia = strtok(data, "/");
    mes = strtok(NULL, "/");
    ano = strtok(NULL, "\0");

    if(strlen(dia) != 2 || strlen(mes) != 2 || strlen(ano) != 4){
        return 0;
    }

    diaNum = strtol(dia, NULL, 10);
    mesNum = strtol(mes, NULL, 10);
    anoNum = strtol(ano, NULL, 10);

    if((diaNum < 1 || diaNum > 31) || (mesNum < 1 || mesNum > 12) || (anoNum < 2011 || anoNum > 2015)){
        return 0;
    }

    return 1;
}

//função para validar se a duração da partida está no formato MM:SS
int validaDuracao(const char string[]){
    char *minuto, *segundo, *duracao;

    duracao = malloc(sizeof(string));
    strcpy(duracao, string);

    minuto = strtok(duracao, ":");
    segundo = strtok(NULL, "\0");

    if(strlen(minuto) != 2 || strlen(segundo) != 2){
        return 0;
    }

    return 1;
}

//função para validar se o placar possui 2 bytes
int validaPlacar(const char string[]){
    if(strlen(string) == 2){
        return 1;
    } else{
        return 0;
    }
}

//função para gerar a chave primária
void gerarCod(Partida *p){
    int i;

    p->codigo[0] = p->equipeAzul[0];
    p->codigo[1] = p->equipeVermelha[0];
    p->codigo[2] = p->mvp[0];
    p->codigo[3] = p->mvp[1];
    p->codigo[4] = p->data[0];
    p->codigo[5] = p->data[1];
    p->codigo[6] = p->data[3];
    p->codigo[7] = p->data[4];
    p->codigo[8]= '\0';
    for(i = 0; i < 8; i++){
        p->codigo[i] = toupper(p->codigo[i]);
    }
}

//função para criar a string de 192 bytes com os dados da partida separados por @, e # preenchendo o espaço que sobra
void criarRegistro(Partida *p, char registro[]){
    size_t tamanho;
    int i;

    strcat(registro, p->codigo);
    strcat(registro, "@");
    strcat(registro, p->equipeAzul);
    strcat(registro, "@");
    strcat(registro, p->equipeVermelha);
    strcat(registro, "@");
    strcat(registro, p->data);
    strcat(registro, "@");
    strcat(registro, p->duracao);
    strcat(registro, "@");
    strcat(registro, p->equipeVencedora);
    strcat(registro, "@");
    strcat(registro, p->placarAzul);
    strcat(registro, "@");
    strcat(registro, p->placarVermelho);
    strcat(registro, "@");
    strcat(registro, p->mvp);
    strcat(registro, "@");

    tamanho = strlen(registro);
    for(i = tamanho; i < 192; i++){
        strcat(registro, "#");
    }
    registro[192] = '\0';
}

//verifica se a chave primária recebida já existe
int verificarChave(RegistroPrim **idxPrim, char chave[], long count){
    long i;

    for (i = 0; i < count; i++) {
        if(strcmp(chave, (*idxPrim)[i].codigo) == 0){
            return 0;
        }
    }

    return 1;
}

//recebe os dados de uma nova partida, válida e insere caso os dados sejam consistentes
long inserirPartida(RegistroPrim **idxPrim, RegistroWin **idxWin, RegistroMvp **idxMvp, long count){
    FILE *matches;
    Partida p;
    char registro[193], erro[36] = "Campo inválido! Informe novamente: ";

    registro[0] = '\0';

    meuGet(p.equipeAzul, erro, 39);
    meuGet(p.equipeVermelha, erro, 39);

    meuGet(p.data, erro, 10);
    while (!validaData(p.data)) {
        printf("%s", erro);
        meuGet(p.data, erro, 10);
    }

    meuGet(p.duracao, erro, 5);
    while (!validaDuracao(p.duracao)) {
        printf("%s", erro);
        meuGet(p.duracao, erro, 5);
    }

    meuGet(p.equipeVencedora, erro, 39);

    while (strcmp(p.equipeVencedora, p.equipeAzul) != 0 && strcmp(p.equipeVencedora, p.equipeVermelha) != 0) {
        printf("%s", erro);
        meuGet(p.equipeVencedora, erro, 39);
    }

    meuGet(p.placarAzul, erro, 2);

    while (!validaPlacar(p.placarAzul)) {
        printf("%s", erro);
        meuGet(p.placarAzul, erro, 2);
    }

    meuGet(p.placarVermelho, erro, 2);

    while (!validaPlacar(p.placarVermelho)) {
        printf("%s", erro);
        meuGet(p.placarVermelho, erro, 2);
    }

    meuGet(p.mvp, erro, 39);
    gerarCod(&p);

    if(verificarChave(idxPrim, p.codigo, count)){
        criarRegistro(&p, registro);
        matches = fopen(ARQ_DADOS, "a+");
        fprintf(matches, "%s", registro);
        count++;
        (*idxPrim) = realloc((*idxPrim), count * sizeof(RegistroPrim));
        (*idxWin) = realloc((*idxWin), count * sizeof(RegistroWin));
        (*idxMvp) = realloc((*idxMvp), count * sizeof(RegistroMvp));
        strcpy((*idxPrim)[count-1].codigo, p.codigo);
        (*idxPrim)[count-1].rrn = (count - 1) * 192;
        strcpy((*idxWin)[count-1].equipeVencedora, p.equipeVencedora);
        strcpy((*idxWin)[count-1].codigo, p.codigo);
        strcpy((*idxMvp)[count-1].mvp, p.mvp);
        strcpy((*idxMvp)[count-1].codigo, p.codigo);
        qsort((*idxPrim), count, sizeof(RegistroPrim), comparePrim);
        qsort((*idxWin), count, sizeof(RegistroWin), compareWin);
        qsort((*idxMvp), count, sizeof(RegistroMvp), compareMvp);
        fclose(matches);
    } else{
        printf("ERRO: Já existe um registro com a chave primária: %s.\n", p.codigo);
    }

    return count;
}

//função de busca binária no índice primário
long buscaBin(RegistroPrim **idxPrim, char chave[], long min, long max){
    long meio;

    while (min <= max) {
        meio = (max + min) / 2;
        if (strcmp(chave, (*idxPrim)[meio].codigo) == 0) {
            return meio;
        } else if (strcmp(chave, (*idxPrim)[meio].codigo) < 1) {
            max = meio - 1;
        } else {
            min = meio + 1;
        }
    }

    return -1;
}

//função para alterar a duração da partida desejada, caso ela exista
void alterarDuracao(RegistroPrim **idxPrim, RegistroPrim *chave, long count){
    FILE *matches;
    char c, duracao[6], erro[16] = "Campo inválido!";
    int i, tokCount = 0;
    long idx;

    if((idx = buscaBin(idxPrim, chave->codigo, 0, count - 1)) != -1){
        meuGet(duracao, erro, 5);
        while (!validaDuracao(duracao)) {
            printf("Campo inválido!");
            meuGet(duracao, erro, 5);
        }

        matches = fopen(ARQ_DADOS, "r+");
        fseek(matches, (*idxPrim)[idx].rrn, SEEK_SET);

        for (i = 0; i < 102 && tokCount < 4; i++) {
            if ((c = fgetc(matches)) == '@') {
                tokCount++;
            }
        }

        fprintf(matches, "%s", duracao);
        fclose(matches);
    } else{
        printf("Registro não encontrado!\n");
    }
}

//função para remover a partida desejada, caso ela exista
void removerPartida(RegistroPrim **idxPrim, RegistroPrim *chave, long count){
    FILE *matches;
    long idx;

    if((idx = buscaBin(idxPrim, chave->codigo, 0, count - 1)) != -1){
        matches = fopen(ARQ_DADOS, "r+");
        fseek(matches, (*idxPrim)[idx].rrn, SEEK_SET);
        fprintf(matches, "*|");
        (*idxPrim)[idx].rrn = -1;
        fclose(matches);
    } else{
        printf("Registro não encontrado!\n");
    }
}

//função para buscar e exibir a partida desejada à partir da chave primária, caso ela exista
void buscarPrim(RegistroPrim **idxPrim, RegistroPrim *chave, long count){
    FILE *matches;
    Partida p;
    long idx;

    if((idx = buscaBin(idxPrim, chave->codigo, 0, count - 1)) != -1){
        matches = fopen(ARQ_DADOS, "r+");
        fseek(matches, (*idxPrim)[idx].rrn, SEEK_SET);
        fscanf(matches, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
        printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
        fclose(matches);
    } else{
        printf("Registro não encontrado!\n");
    }
}

//função para buscar e exibir a partida desejada à partir da equipe vencedora, caso ela exista
void buscarWin(RegistroPrim **idxPrim, RegistroWin **idxWin, char equipe[], long count){
    FILE *matches;
    Partida p;
    RegistroPrim chaveBuscada;
    int flag = 1;
    long i, idx;

    matches = fopen(ARQ_DADOS, "r+");

    for (i = 0; i < count; i++) {
        if(strcmp(equipe, (*idxWin)[i].equipeVencedora) == 0){
            strcpy(chaveBuscada.codigo, (*idxWin)[i].codigo);
            idx = buscaBin(idxPrim, chaveBuscada.codigo, 0, count - 1);
            if((*idxPrim)[idx].rrn != -1){
                fseek(matches, (*idxPrim)[idx].rrn, SEEK_SET);
                fscanf(matches, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
                printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
                flag = 0;
            }
        } else if(strcmp(equipe, (*idxWin)[i].equipeVencedora) < 0){
            break;
        }
    }

    if (flag) {
        printf("Registro não encontrado!\n");
    }

    fclose(matches);
}

//função para buscar e exibir a partida desejada à partir do mvp, caso ela exista
void buscarMvp(RegistroPrim **idxPrim, RegistroMvp **idxMvp, char mvp[], long count){
    FILE *matches;
    Partida p;
    RegistroPrim chaveBuscada;
    int flag = 1;
    long i, idx;

    matches = fopen(ARQ_DADOS, "r+");

    for (i = 0; i < count; i++) {
        if(strcmp(mvp, (*idxMvp)[i].mvp) == 0){
            strcpy(chaveBuscada.codigo, (*idxMvp)[i].codigo);
            idx = buscaBin(idxPrim, chaveBuscada.codigo, 0, count - 1);
            if((*idxPrim)[idx].rrn != -1){
                fseek(matches, (*idxPrim)[idx].rrn, SEEK_SET);
                fscanf(matches, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
                printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
                flag = 0;
            }
        } else if(strcmp(mvp, (*idxMvp)[i].mvp) < 0){
            break;
        }
    }

    if (flag) {
        printf("Registro não encontrado!\n");
    }

    fclose(matches);
}

//função para listar as partidas na ordem em que estão no índice primário
void listarPrim(RegistroPrim **idxPrim, long count){
    FILE *matches;
    Partida p;
    int flag = 1;
    long i;

    matches = fopen(ARQ_DADOS, "r+");

    for (i = 0; i < count; i++) {
        if ((*idxPrim)[i].rrn != -1) {
            fseek(matches, (*idxPrim)[i].rrn, SEEK_SET);
            fscanf(matches, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
            printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
            flag = 0;
        }
    }

    if (flag) {
        printf("Arquivo vazio!");
    }

    fclose(matches);
}

//função para listar as partidas na ordem em que estão no índice secundário de equipe vencedora
void listarWin(RegistroPrim **idxPrim, RegistroWin **idxWin, long count){
    FILE *matches;
    Partida p;
    RegistroPrim chaveBuscada;
    int flag = 1;
    long i, idx;

    matches = fopen(ARQ_DADOS, "r+");

    for (i = 0; i < count; i++) {
        strcpy(chaveBuscada.codigo, (*idxWin)[i].codigo);
        idx = buscaBin(idxPrim, chaveBuscada.codigo, 0, count - 1);
        if((*idxPrim)[idx].rrn != -1){
            fseek(matches, (*idxPrim)[idx].rrn, SEEK_SET);
            fscanf(matches, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
            printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
            flag = 0;
        }
    }

    if (flag) {
        printf("Arquivo vazio!");
    }

    fclose(matches);
}

//função para listar as partidas na ordem em que estão no índice secundário de mvp
void listarMvp(RegistroPrim **idxPrim, RegistroMvp **idxMvp, long count){
    FILE *matches;
    Partida p;
    RegistroPrim chaveBuscada;
    int flag = 1;
    long i, idx;

    matches = fopen(ARQ_DADOS, "r+");

    for (i = 0; i < count; i++) {
        strcpy(chaveBuscada.codigo, (*idxMvp)[i].codigo);
        idx = buscaBin(idxPrim, chaveBuscada.codigo, 0, count - 1);
        if((*idxPrim)[idx].rrn != -1){
            fseek(matches, (*idxPrim)[idx].rrn, SEEK_SET);
            fscanf(matches, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
            printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n", p.codigo, p.equipeAzul, p.equipeVermelha, p.data, p.duracao, p.equipeVencedora, p.placarAzul, p.placarVermelho, p.mvp);
            flag = 0;
        }
    }

    if (flag) {
        printf("Arquivo vazio!");
    }

    fclose(matches);
}

//função para limpar do arquivo de dados os registros marcados como removidos, e removê-los também dos índices, atualizando os rrns no índice primário
long liberar(RegistroPrim **idxPrim, RegistroWin **idxWin, RegistroMvp **idxMvp, long count){
    FILE *matches, *temp;
    char registro[193], chave[9];
    long i, j, k, idx;
    size_t arqSize;

    matches = fopen(ARQ_DADOS, "r+");

    //arquivo temporário para armazenar os registros
    temp = fopen(ARQ_TEMP, "w+");

    //armazenando o tamanho do arquivo de dados em variável
    fseek(matches, 0, SEEK_END);
    arqSize = ftell(matches);
    fseek(matches, 0, SEEK_SET);

    while (ftell(matches) < arqSize) {
        fscanf(matches, "%192[^\n]", registro);
        if(registro[0] != '*' || registro[1] != '|'){
            fprintf(temp, "%s", registro);
        }
    }



    for (i = 0; i < count; i++) {
        if((*idxPrim)[i].rrn == -1){
            for (j = 0; j < count; j++) {
                if (strcmp((*idxPrim)[i].codigo, (*idxWin)[j].codigo) == 0) {
                    for (k = j + 1; k < count; k++) {
                        (*idxWin)[k - 1] = (*idxWin)[k];
                    }
                }
            }

            for (j = 0; j < count; j++) {
                if (strcmp((*idxPrim)[i].codigo, (*idxMvp)[j].codigo) == 0) {
                    for (k = j + 1; k < count; k++) {
                        (*idxMvp)[k - 1] = (*idxMvp)[k];
                    }
                }
            }

            for (j = i + 1; j < count; j++) {
                (*idxPrim)[j - 1] = (*idxPrim)[j];
            }

            count--;
        }
    }

    fseek(temp, 0, SEEK_END);
    arqSize = ftell(temp);
    fseek(temp, 0, SEEK_SET);

    while (ftell(temp) < arqSize) {
        fscanf(temp, "%[^@]%*c", chave);
        idx = buscaBin(idxPrim, chave, 0, count);
        (*idxPrim)[idx].rrn = (ftell(temp) - 9);
        fseek(temp, 183, SEEK_CUR);
    }

    fclose(matches);
    fclose(temp);

    rename(ARQ_TEMP, ARQ_DADOS);

    return count;
}

//função para gravar os índices da memória principal para a memória secundário no fim do programa
void gravarIdx(RegistroPrim **idxPrim, RegistroWin **idxWin, RegistroMvp **idxMvp, long count){
    FILE *iprimary, *iwinner, *imvp;
    long i;

    iprimary = fopen(IDX_PRIM, "w+");
    iwinner = fopen(IDX_WIN, "w+");
    imvp = fopen(IDX_MVP, "w+");

    fseek(iprimary, 2, SEEK_SET);
    for (i = 0; i < count; i++) {
        fprintf(iprimary, "%s %ld\n", (*idxPrim)[i].codigo, (*idxPrim)[i].rrn);
        fprintf(iwinner, "%s@%s\n", (*idxWin)[i].equipeVencedora, (*idxWin)[i].codigo);
        fprintf(imvp, "%s@%s\n", (*idxMvp)[i].mvp, (*idxMvp)[i].codigo);
    }
    fseek(iprimary, 0, SEEK_SET);
    fprintf(iprimary, "0\n");

    fclose(iprimary);
    fclose(iwinner);
    fclose(imvp);
}
