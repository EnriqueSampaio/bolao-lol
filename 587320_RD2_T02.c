/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 587320
 * Aluno: Enrique Sampaio dos Santos
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 9
#define TAM_EQUIPE 40
#define TAM_DATA 11
#define TAM_DURACAO 6
#define TAM_PLACAR 3
#define TAM_MVP 40

#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO "Registro nao encontrado!\n\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define NOS_PERCORRIDOS "Busca por %s. Nos percorridos:\n"

/* Registro da partida */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char equipe_azul[TAM_EQUIPE];
	char equipe_vermelha[TAM_EQUIPE];
	char data_partida[TAM_DATA];	// DD/MM/AAAA
	char duracao[TAM_DURACAO];			// MM:SS
	char vencedor[TAM_EQUIPE];
	char placar1[TAM_PLACAR];
	char placar2[TAM_PLACAR];
	char mvp[TAM_MVP];
} Partida;

/* Registro da Árvore-B
 * Contém a chave primária e o RRN do respectivo registro */
typedef struct {
	char pk[TAM_PRIMARY_KEY];	// chave primária
	int rrn;					// rrn do registro
} Chave;

/* Estrutura da Árvore-B */
typedef struct node node_Btree;
struct node {
	int num_chaves;		// numero de chaves armazenadas
	Chave *chave;		// vetor das chaves e rrns [m-1]
	node_Btree **desc;	// ponteiros para os descendentes, *desc[m]
	int folha;			// flag folha da arvore
};
typedef struct {
	node_Btree *raiz;
} Iprimary;

/* Registro do índice secundário
 * Contém o nome da equipe vencedora e a chave primária do registro */
typedef struct {
	char vencedor[TAM_EQUIPE];
	char pk[TAM_PRIMARY_KEY];
} Iwinner;

/* Registro do índice secundário
 * Contém o apelido do MVP e a chave primária do registro */
typedef struct {
	char mvp[TAM_MVP];
	char pk[TAM_PRIMARY_KEY];
} Imvp;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int M;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe o jogador */
void exibir_registro(int rrn);

/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
/* Divide nó de promove uma chave */
void divide_no(node_Btree *no, Chave *chave, node_Btree **filhoDireito, Chave **chavePromovida);

/* Abre espaço no registro para o novo nó */
void insere_aux(node_Btree *no, Chave *chave, node_Btree **filhoDireito, Chave **chavePromovida);

/* Insere nó na árvore B*/
void insere(Iprimary *iprimary, Chave *chave);

/* Recupera todos os dados de uma partida do arquivo de dados */
Partida recuperar_registro(int rrn);

/* Gera o índice primário na estrutura de Árvore-B */
void criar_iprimary(Iprimary *iprimary, int nregistros, int ordem);

/* Gera o índice secundário de equipe vencedora */
void criar_iwinner(Iwinner *iwinner, int nregistros);

/* Gera o índice secundário de MVP */
void criar_imvp(Imvp *imvp, int nregistros);

/* Recebe uma string até chegar no \n */
void meuGet(char string[], char erro[], int limite);

/* Valida se a data foi inserida corretamente */
int validaData(const char string[]);

/* Valida se a duração foi inserida corretamente */
int validaDuracao(const char string[]);

/* Valida se o placar foi inserido corretamente */
int validaPlacar(const char string[]);

/* Gera o código da chave primária */
void gerarCod(Partida *p);

/* Cria o registro para ser inserido no arquivo de dados */
void criarRegistro(Partida *p, char registro[]);

/* Verifica se a chave é nova ou já existente */
int verificarChave(Iprimary *iprimary, char chave[], int nregistros);

/* Cadastra nova partida */
void cadastrar(Iprimary *iprimary, Iwinner *iwinner, Imvp *imvp, int *nregistros);

/* Altera o tempo de duração da partida */
void alterar(Iprimary iprimary);

/* Busca partida */
void buscar(Iprimary iprimary, Iwinner *iwinner, Imvp *imvp, int nregistros);

/* Lista partida */
void listar(Iprimary iprimary, Iwinner *iwinner, Imvp *imvp, int nregistros);

/* Apaga nó da Árvore-B (índice primário) */
void apagar_no(node_Btree **raiz);

/* Ignora a entrada */
void ignore();

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main() {

	/* Arquivo */
	int carregarArquivo = 0, nregistros = 0;
	scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) {
		nregistros = carregar_arquivo();
	}


	/* Índice primário */
	int ordem;
	scanf("%d", &ordem);
	Iprimary iprimary;
	criar_iprimary(&iprimary, nregistros, ordem);


	/* Índice secundário de vencedores */
	Iwinner *iwinner = (Iwinner *) malloc (MAX_REGISTROS * sizeof(Iwinner));
	if (!iwinner) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iwinner(iwinner, nregistros);


	/* Índice secundário de MVPs */
	Imvp *imvp = (Imvp *) malloc (MAX_REGISTROS * sizeof(Imvp));
	if (!imvp) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_imvp(imvp, nregistros);


	/* Execução do programa */
	int opcao = 0;
	while(opcao != 5) {
		scanf("%d", &opcao);
		switch(opcao) {

		case 1:
			getchar();
			cadastrar(&iprimary, iwinner, imvp, &nregistros);
			break;
		case 2:
			getchar();
			alterar(iprimary);
			break;
		case 3:
			buscar(iprimary, iwinner, imvp, nregistros);
			break;
		case 4:
			listar(iprimary, iwinner, imvp, nregistros);
			break;

		case 5: /* Libera memoria alocada */
			apagar_no(&iprimary.raiz);
			free(iwinner);
			free(imvp);
			break;

		case 10:
			printf("%s\n", ARQUIVO);
			break;

		default:
			ignore();
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}


/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Exibe a partida */
void exibir_registro(int rrn) {

	Partida j = recuperar_registro(rrn);

	printf("%s\n", j.pk);
	printf("%s\n", j.equipe_azul);
	printf("%s\n", j.equipe_vermelha);
	printf("%s\n", j.data_partida);
	printf("%s\n", j.duracao);
	printf("%s\n", j.vencedor);
	printf("%s\n", j.placar1);
	printf("%s\n", j.placar2);
	printf("%s\n", j.mvp);
	printf("\n");
}

/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */
void divide_no(node_Btree *no, Chave *chave, node_Btree **filhoDireito, Chave **chavePromovida) {
	int i, j, chaveAlocada;
	node_Btree *novo;

	i = no->num_chaves;
	chaveAlocada = 0;
	novo = (node_Btree *) malloc(sizeof(node_Btree));
	novo->chave = (Chave *) malloc((M - 1) * sizeof(Chave));
	novo->desc = (node_Btree**) malloc(M * sizeof(node_Btree*));
	novo->folha = no->folha;
	novo->num_chaves = (M - 1) / 2;

	for (j = novo->num_chaves - 1; j >= 0; j--) {
		if (!chaveAlocada && (strcmp(chave->pk, no->chave[i].pk) > 0)) {
			novo->chave[j] = *chave;
			if (filhoDireito != NULL) {
				novo->desc[j + 1] = (*filhoDireito);
			} else {
				novo->desc[j + 1] = NULL;
			}
			chaveAlocada = 1;
		} else {
			novo->chave[j] = no->chave[i];
			novo->desc[j + 1] = no->desc[i + 1];
			i--;
		}
	}

	if (!chaveAlocada) {
		while (i >= 0 && (strcmp(chave->pk, no->chave[i].pk) < 0)) {
			no->chave[i + 1] = no->chave[i];
			no->desc[i + 2] = no->desc[i + 1];
			i--;
		}
		no->chave[i + 1] = *chave;
		no->desc[i + 2] = (*filhoDireito);
	}

	**chavePromovida = no->chave[(M/2) + 1];
	novo->desc[1] = no->desc[(M/2) + 2];
	no->num_chaves = M / 2;
	filhoDireito = &novo;
}

void insere_aux(node_Btree *no, Chave *chave, node_Btree **filhoDireito, Chave **chavePromovida) {
	int i;

	if (no->folha) {
		if (no->num_chaves < M - 1) {
			i = no->num_chaves - 1;
			while (i >= 0 && (strcmp(chave->pk, no->chave[i].pk) < 0)) {
				no->chave[i + 1] = no->chave[i];
				i--;
			}
			no->chave[i +1] = *chave;
			no->num_chaves = no->num_chaves + 1;
			(*chavePromovida) = NULL;
			(*filhoDireito) = NULL;
		} else {
			divide_no(no, chave, NULL, chavePromovida);
		}
	} else{
		printf("%d\n", no->num_chaves);
		i = no->num_chaves;
		while (i >= 0 && (strcmp(chave->pk, no->chave[i].pk) < 0)) {
			i--;
		}
		i++;

		insere_aux(no->desc[i], chave, filhoDireito, chavePromovida);

		if (chavePromovida != NULL) {
			chave = (*chavePromovida);
			if (no->num_chaves < M - 1) {
				i = no->num_chaves - 1;
				while (i >= 0 && (strcmp(chave->pk, no->chave[i].pk) < 0)) {
					no->chave[i + 1] = no->chave[i];
					no->desc[i + 2] = no->desc[i + 1];
					i--;
				}
				no->chave[i + 1] = *chave;
				if (filhoDireito != NULL) {
					no->desc[i + 2] = (*filhoDireito);
				} else {
					no->desc[i + 2] = NULL;
				}
				no->num_chaves++;
				chavePromovida = NULL;
				filhoDireito = NULL;
			} else {
				divide_no(no, chave, filhoDireito, chavePromovida);
			}
		} else {
			chavePromovida = NULL;
			filhoDireito = NULL;
		}
	}
}

void insere(Iprimary *iprimary, Chave *chave) {
	Chave *chavePromovida;
	node_Btree *novo, *filhoDireito;

	if (iprimary->raiz == NULL) {
		novo = (node_Btree *) malloc(sizeof(node_Btree));
		novo->folha = 1;
		novo->num_chaves = 1;
		novo->chave = (Chave *) malloc((M - 1) * sizeof(Chave));
		novo->desc = (node_Btree**) malloc(M * sizeof(node_Btree*));
		novo->chave[0] = *chave;
		iprimary->raiz = novo;
	} else {
		insere_aux(iprimary->raiz, chave, &filhoDireito, &chavePromovida);
		if (chavePromovida != NULL) {
			novo = (node_Btree *) malloc(sizeof(node_Btree));
			novo->folha = 0;
			novo->num_chaves = 1;
			novo->chave = (Chave *) malloc((M - 1) * sizeof(Chave));
			novo->desc = (node_Btree**) malloc(M * sizeof(node_Btree*));
			novo->chave[0] = *chavePromovida;
			novo->desc[0] = iprimary->raiz;
			novo->desc[1] = filhoDireito;
			iprimary->raiz = novo;
		}
	}
}

Partida recuperar_registro(int rrn){
	char buffer[TAM_REGISTRO +1];
	Partida p;

	*buffer = *(ARQUIVO + rrn);
	sscanf(buffer, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", p.pk, p.equipe_azul, p.equipe_vermelha, p.data_partida, p.duracao, p.vencedor, p.placar1, p.placar2, p.mvp);
	return p;
}

void criar_iprimary(Iprimary *iprimary, int nregistros, int ordem) {
	char buffer[TAM_REGISTRO + 1];
	Chave aux;
	int i, j;

	M = ordem;

	i = 0;
	iprimary->raiz = NULL;
	for (j = 0; j < nregistros; j++) {
		*buffer = *(ARQUIVO + i);
		sscanf(buffer, "%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c", aux.pk);
		aux.rrn = i;
		insere(iprimary, &aux);
		i+= 192;
	}
}

void criar_iwinner(Iwinner *iwinner, int nregistros) {
	char buffer[TAM_REGISTRO + 1];
	int i, j;

	i = 0;
	for (j = 0; j < nregistros; j++) {
		*buffer = *(ARQUIVO + i);
		sscanf(buffer, "%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c", iwinner[j].pk, iwinner[j].vencedor);
		i+= 192;
	}
	//PRECISA ORDENAR
}

void criar_imvp(Imvp *imvp, int nregistros) {
	char buffer[TAM_REGISTRO + 1];
	int i, j;

	i = 0;
	for (j = 0; j < nregistros; j++) {
		*buffer = *(ARQUIVO + i);
		sscanf(buffer, "%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%[^@]%*c", imvp[j].pk, imvp[j].mvp);
		i+= 192;
	}
	//PRECISA ORDENAR
}

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

int validaPlacar(const char string[]){
    if(strlen(string) == 2){
        return 1;
    } else{
        return 0;
    }
}

void gerarCod(Partida *p){
    int i;

    p->pk[0] = p->equipe_azul[0];
    p->pk[1] = p->equipe_vermelha[0];
    p->pk[2] = p->mvp[0];
    p->pk[3] = p->mvp[1];
    p->pk[4] = p->data_partida[0];
    p->pk[5] = p->data_partida[1];
    p->pk[6] = p->data_partida[3];
    p->pk[7] = p->data_partida[4];
    p->pk[8]= '\0';
    for(i = 0; i < 8; i++){
        p->pk[i] = toupper(p->pk[i]);
    }
}

void criarRegistro(Partida *p, char registro[]){
    size_t tamanho;
    int i;

    strcat(registro, p->pk);
    strcat(registro, "@");
    strcat(registro, p->equipe_azul);
    strcat(registro, "@");
    strcat(registro, p->equipe_vermelha);
    strcat(registro, "@");
    strcat(registro, p->data_partida);
    strcat(registro, "@");
    strcat(registro, p->duracao);
    strcat(registro, "@");
    strcat(registro, p->vencedor);
    strcat(registro, "@");
    strcat(registro, p->placar1);
    strcat(registro, "@");
    strcat(registro, p->placar2);
    strcat(registro, "@");
    strcat(registro, p->mvp);
    strcat(registro, "@");

    tamanho = strlen(registro);
    for(i = tamanho; i < 192; i++){
        strcat(registro, "#");
    }
    registro[192] = '\0';
}

int verificarChave(Iprimary *iprimary, char chave[], int nregistros){
	//busca na Árvore-B, se encontrar, retorna 0, senão, 1
}

void cadastrar(Iprimary *iprimary, Iwinner *iwinner, Imvp *imvp, int *nregistros) {
	Partida p;
    char registro[193];
	Chave chave;

    registro[0] = '\0';

    meuGet(p.equipe_azul, CAMPO_INVALIDO, 39);
    meuGet(p.equipe_vermelha, CAMPO_INVALIDO, 39);

    meuGet(p.data_partida, CAMPO_INVALIDO, 10);
    while (!validaData(p.data_partida)) {
        printf("%s", CAMPO_INVALIDO);
        meuGet(p.data_partida, CAMPO_INVALIDO, 10);
    }

    meuGet(p.duracao, CAMPO_INVALIDO, 5);
    while (!validaDuracao(p.duracao)) {
        printf("%s", CAMPO_INVALIDO);
        meuGet(p.duracao, CAMPO_INVALIDO, 5);
    }

    meuGet(p.vencedor, CAMPO_INVALIDO, 39);

    while (strcmp(p.vencedor, p.equipe_azul) != 0 && strcmp(p.vencedor, p.equipe_vermelha) != 0) {
        printf("%s", CAMPO_INVALIDO);
        meuGet(p.vencedor, CAMPO_INVALIDO, 39);
    }

    meuGet(p.placar1, CAMPO_INVALIDO, 2);

    while (!validaPlacar(p.placar1)) {
        printf("%s", CAMPO_INVALIDO);
        meuGet(p.placar1, CAMPO_INVALIDO, 2);
    }

    meuGet(p.placar2, CAMPO_INVALIDO, 2);

    while (!validaPlacar(p.placar2)) {
        printf("%s", CAMPO_INVALIDO);
        meuGet(p.placar2, CAMPO_INVALIDO, 2);
    }

    meuGet(p.mvp, CAMPO_INVALIDO, 39);
    gerarCod(&p);

    if(verificarChave(iprimary, p.pk, *nregistros)){
        criarRegistro(&p, registro);
		sprintf(ARQUIVO, "%s", registro);
		strcpy(chave.pk, p.pk);
		chave.rrn = *nregistros * 192;
		insere(iprimary, &chave);
        strcpy(iwinner[*nregistros].vencedor, p.vencedor);
        strcpy(iwinner[*nregistros].pk, p.pk);
        strcpy(imvp[*nregistros].mvp, p.mvp);
        strcpy(imvp[*nregistros].pk, p.pk);
		*nregistros++;
		//PRECISA ORDENAR
    } else{
        printf(ERRO_PK_REPETIDA, p.pk);
    }
}

void alterar(Iprimary iprimary) {

}

void buscar(Iprimary iprimary, Iwinner *iwinner, Imvp *imvp, int nregistros) {

}

void listar(Iprimary iprimary, Iwinner *iwinner, Imvp *imvp, int nregistros) {

}

void apagar_no(node_Btree **raiz) {

}

void ignore() {

}
