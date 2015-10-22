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

/* Recupera todos os dados de uma partida do arquivo de dados */
Partida recuperar_registro(int rrn);

/* Divide nó de promove uma chave */
node_Btree* divide_no(node_Btree **no, Chave *chave, node_Btree **filhoDireito, Chave **chavePromovida);

/* Abre espaço no registro para o novo nó */
node_Btree* insere_aux(node_Btree **no, Chave *chave, Chave **chavePromovida);

/* Insere nó na árvore B*/
void insere(Iprimary *iprimary, Chave *chave);

/* Retorna o nó que contém a chave com determinado pk */
node_Btree* buscar_arvore(node_Btree **no, char pk[], int listarCaminho);

/* Recupera todas partidas com determinado vencedor */
void buscar_vencedor(Iprimary *iprimary, Iwinner *iwinner, char vencedor[], int nregistros);

/* Recupera todas partidas com determinado MVP */
void buscar_mvp(Iprimary *iprimary, Imvp *imvp, char mvp[], int nregistros);

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
int verificarChave(Iprimary *iprimary, char chave[]);

/* Cadastra nova partida */
void cadastrar(Iprimary *iprimary, Iwinner *iwinner, Imvp *imvp, int *nregistros);

/* Altera o tempo de duração da partida */
void alterar(Iprimary iprimary);

/* Busca partida */
void buscar(Iprimary iprimary, Iwinner *iwinner, Imvp *imvp, int nregistros);

/* Lê a Árvore em preordem */
void preordem(node_Btree **no, int nivel);

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

Partida recuperar_registro(int rrn) {
	char *buffer;
	Partida p;

	buffer = ARQUIVO + rrn;
	sscanf(buffer, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", p.pk, p.equipe_azul, p.equipe_vermelha, p.data_partida, p.duracao, p.vencedor, p.placar1, p.placar2, p.mvp);
	return p;
}

node_Btree* divide_no(node_Btree **no, Chave *chave, node_Btree **filhoDireito, Chave **chavePromovida) {
	int i, j, chaveAlocada;
	node_Btree *novo;

	i = (*no)->num_chaves - 1;
	chaveAlocada = 0;
	novo = (node_Btree *) malloc(sizeof(node_Btree));
	novo->chave = (Chave *) malloc((M - 1) * sizeof(Chave));
	novo->desc = (node_Btree**) malloc(M * sizeof(node_Btree*));
	novo->folha = (*no)->folha;
	novo->num_chaves = (M - 1) / 2;

	for (j = novo->num_chaves - 1; j >= 0; j--) {
		if (!chaveAlocada && (strcmp(chave->pk, (*no)->chave[i].pk) > 0)) {
			novo->chave[j] = *chave;
			if (filhoDireito != NULL) {
				novo->desc[j + 1] = (*filhoDireito);
			} else {
				novo->desc[j + 1] = NULL;
			}
			chaveAlocada = 1;
		} else {
			novo->chave[j] = (*no)->chave[i];
			novo->desc[j + 1] = (*no)->desc[i + 1];
			i--;
		}
	}

	if (!chaveAlocada) {
		while (i >= 0 && (strcmp(chave->pk, (*no)->chave[i].pk) < 0)) {
			(*no)->chave[i + 1] = (*no)->chave[i];
			(*no)->desc[i + 2] = (*no)->desc[i + 1];
			i--;
		}
		(*no)->chave[i + 1] = *chave;
		if (filhoDireito != NULL) {
			(*no)->desc[i + 2] = (*filhoDireito);
		} else {
			(*no)->desc[i + 2] = NULL;
		}
	}

    (*chavePromovida) = (Chave *) malloc(sizeof(Chave));
	**chavePromovida = (*no)->chave[(M/2)];
	novo->desc[0] = (*no)->desc[(M/2) + 1];
	(*no)->num_chaves = M / 2;
	return novo;
}

node_Btree* insere_aux(node_Btree **no, Chave *chave, Chave **chavePromovida) {
	int i;
	node_Btree *filhoDireito;

	if ((*no)->folha) {
		if ((*no)->num_chaves < M - 1) {
			i = (*no)->num_chaves - 1;
			while (i >= 0 && (strcmp(chave->pk, (*no)->chave[i].pk) < 0)) {
				(*no)->chave[i + 1] = (*no)->chave[i];
				i--;
			}
			(*no)->chave[i +1] = *chave;
			(*no)->num_chaves = (*no)->num_chaves + 1;
			(*chavePromovida) = NULL;
			return NULL;
		} else {
			return divide_no(no, chave, NULL, chavePromovida);
		}
	} else{
		i = (*no)->num_chaves - 1;
		while (i >= 0 && (strcmp(chave->pk, (*no)->chave[i].pk) < 0)) {
			i--;
		}
		i++;
		filhoDireito = (node_Btree *) malloc(sizeof(node_Btree));
		filhoDireito = insere_aux(&(*no)->desc[i], chave, chavePromovida);

		if ((*chavePromovida) != NULL) {
			chave = (*chavePromovida);
			if ((*no)->num_chaves < M - 1) {
				i = (*no)->num_chaves - 1;
				while (i >= 0 && (strcmp(chave->pk, (*no)->chave[i].pk) < 0)) {
					(*no)->chave[i + 1] = (*no)->chave[i];
					(*no)->desc[i + 2] = (*no)->desc[i + 1];
					i--;
				}
				(*no)->chave[i + 1] = *chave;
				if (filhoDireito != NULL) {
					(*no)->desc[i + 2] = filhoDireito;
				} else {
					(*no)->desc[i + 2] = NULL;
				}
				(*no)->num_chaves = (*no)->num_chaves + 1;
				(*chavePromovida) = NULL;
				return NULL;
			} else {
				return divide_no(no, chave, &filhoDireito, chavePromovida);
			}
		} else {
			(*chavePromovida) = NULL;
			return NULL;
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
		filhoDireito = malloc(sizeof(node_Btree));
		filhoDireito = insere_aux(&iprimary->raiz, chave, &chavePromovida);
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

node_Btree* buscar_arvore(node_Btree **no, char pk[], int listarCaminho) {
	int i = 0;

	if ((*no) == NULL) {
		return NULL;
	}

	if (listarCaminho) {
        printf("%s", (*no)->chave[0].pk);
		for (i = 1; i < (*no)->num_chaves; i++) {
			printf(", %s", (*no)->chave[i].pk);
		}
        printf("\n");
	}

	i = 0;

	while (i < (*no)->num_chaves && strcmp(pk, (*no)->chave[i].pk) > 0) {
		i++;
	}

	if (i < (*no)->num_chaves && strcmp(pk, (*no)->chave[i].pk) == 0) {
		return (*no);
	}

	if ((*no)->folha) {
		return NULL;
	} else {
		return buscar_arvore(&(*no)->desc[i], pk, listarCaminho);
	}
}

void buscar_vencedor(Iprimary *iprimary, Iwinner *iwinner, char vencedor[], int nregistros) {
	int i, j, flag = 1;
	node_Btree *no;

	for (i = 0; i < nregistros; i++) {
		if (strcmp(vencedor, iwinner[i].vencedor) == 0) {
			flag = 0;
			no = (node_Btree *) malloc(sizeof(node_Btree));
			no = buscar_arvore(&iprimary->raiz, iwinner[i].pk, 0);
			for (j = 0; j < no->num_chaves; j++) {
				if (strcmp(iwinner[i].pk, no->chave[j].pk) == 0) {
					exibir_registro(no->chave[j].rrn);
				}
			}
		}
	}

	if (flag) {
		printf(REGISTRO_N_ENCONTRADO);
	}
}

void buscar_mvp(Iprimary *iprimary, Imvp *imvp, char mvp[], int nregistros) {
	int i, j, flag = 1;
	node_Btree *no;

	for (i = 0; i < nregistros; i++) {
		if (strcmp(mvp, imvp[i].mvp) == 0) {
			flag = 0;
			no = (node_Btree *) malloc(sizeof(node_Btree));
			no = buscar_arvore(&iprimary->raiz, imvp[i].pk, 0);
			for (j = 0; j < no->num_chaves; j++) {
				if (strcmp(imvp[i].pk, no->chave[j].pk) == 0) {
					exibir_registro(no->chave[j].rrn);
				}
			}
		}
	}

	if (flag) {
		printf(REGISTRO_N_ENCONTRADO);
	}
}

void criar_iprimary(Iprimary *iprimary, int nregistros, int ordem) {
	char *buffer;
	Chave aux;
	int i, j;

	M = ordem;

	i = 0;
	iprimary->raiz = NULL;
	for (j = 0; j < nregistros; j++) {
		buffer = ARQUIVO + i;
		sscanf(buffer, "%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c", aux.pk);
		aux.rrn = i;
		insere(iprimary, &aux);
		i+= 192;
	}

}

void criar_iwinner(Iwinner *iwinner, int nregistros) {
	char *buffer;
	int i, j;
	Iwinner temp;

	i = 0;
	for (j = 0; j < nregistros; j++) {
		buffer = ARQUIVO + i;
		sscanf(buffer, "%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c", iwinner[j].pk, iwinner[j].vencedor);
		i+= 192;
	}
	for (i = 0; i < nregistros; i++) {
		temp = iwinner[i];
		j = i - 1;
		while (j >= 0 && (strcmp(temp.vencedor, iwinner[j].vencedor) < 0 || (strcmp(temp.vencedor, iwinner[j].vencedor) == 0 && strcmp(temp.pk, iwinner[j].pk) < 0))) {
			iwinner[j+1] = iwinner[j];
			j--;
		}
		iwinner[j+1] = temp;
	}
}

void criar_imvp(Imvp *imvp, int nregistros) {
	char *buffer;
	int i, j;
	Imvp temp;

	i = 0;
	for (j = 0; j < nregistros; j++) {
		buffer = ARQUIVO + i;
		sscanf(buffer, "%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%[^@]%*c", imvp[j].pk, imvp[j].mvp);
		i+= 192;
	}
	for (i = 0; i < nregistros; i++) {
		temp = imvp[i];
		j = i - 1;
		while (j >= 0 && (strcmp(temp.mvp, imvp[j].mvp) < 0 || (strcmp(temp.mvp, imvp[j].mvp) == 0 && strcmp(temp.pk, imvp[j].pk) < 0))) {
			imvp[j+1] = imvp[j];
			j--;
		}
		imvp[j+1] = temp;
	}
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
	long minutoNum, segundoNum;

    duracao = malloc(sizeof(string));
    strcpy(duracao, string);

    minuto = strtok(duracao, ":");
    segundo = strtok(NULL, "\0");

    if(strlen(minuto) != 2 || strlen(segundo) != 2){
        return 0;
    } else if (minuto[0] < '0' || minuto[0] > '9' || minuto[1] < '0' || minuto[1] > '9' || segundo[0] < '0' || segundo[0] > '9' || segundo[1] < '0' || segundo[1] > '9') {
		return 0;
	}

	minutoNum = strtol(minuto, NULL, 10);
	segundoNum = strtol(segundo, NULL, 10);

	if (minutoNum < 0 || minutoNum > 99 || segundoNum < 0 || segundoNum > 59) {
		return 0;
	}

    return 1;
}

int validaPlacar(const char string[]){
	int i;

    if (strlen(string) == 2) {
		for (i = 0; i < 2; i++) {
			if (string[i] < '0' || string[i] > '9') {
				return 0;
			}
		}
        return 1;
    } else {
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

int verificarChave(Iprimary *iprimary, char pk[]){
	if (buscar_arvore(&iprimary->raiz, pk, 0) != NULL) {
		return 0;
	} else {
		return 1;
	}
}

void cadastrar(Iprimary *iprimary, Iwinner *iwinner, Imvp *imvp, int *nregistros) {
	Partida p;
    char registro[193];
	Chave chave;
	Imvp tempMvp;
	int i, j;
	Iwinner tempWin;

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

    if(verificarChave(iprimary, p.pk)) {
        criarRegistro(&p, registro);
		sprintf(ARQUIVO, "%s", registro);
		strcpy(chave.pk, p.pk);
		chave.rrn = *nregistros * 192;
		insere(iprimary, &chave);
        strcpy(iwinner[*nregistros].vencedor, p.vencedor);
        strcpy(iwinner[*nregistros].pk, p.pk);
        strcpy(imvp[*nregistros].mvp, p.mvp);
        strcpy(imvp[*nregistros].pk, p.pk);
		*nregistros = *nregistros + 1;

		for (i = 0; i < *nregistros; i++) {
			tempWin = iwinner[i];
			j = i - 1;
			while (j >= 0 && (strcmp(tempWin.vencedor, iwinner[j].vencedor) < 0 || (strcmp(tempWin.vencedor, iwinner[j].vencedor) == 0 && strcmp(tempWin.pk, iwinner[j].pk) < 0))) {
				iwinner[j+1] = iwinner[j];
				j--;
			}
			iwinner[j+1] = tempWin;
		}
		for (i = 0; i < *nregistros; i++) {
			tempMvp = imvp[i];
			j = i - 1;
			while (j >= 0 && (strcmp(tempMvp.mvp, imvp[j].mvp) < 0 || (strcmp(tempMvp.mvp, imvp[j].mvp) == 0 && strcmp(tempMvp.pk, imvp[j].pk) < 0))) {
				imvp[j+1] = imvp[j];
				j--;
			}
			imvp[j+1] = tempMvp;
		}
    } else {
        printf(ERRO_PK_REPETIDA, p.pk);
    }
}

void alterar(Iprimary iprimary) {
	char *buffer, duracao[TAM_DURACAO], pk[TAM_PRIMARY_KEY];
	int i, j = 0;
	node_Btree *no;

    scanf("%[^\n]", pk);
    ignore();
	no = (node_Btree *) malloc(sizeof(node_Btree));
	no = buscar_arvore(&iprimary.raiz, pk, 0);
	if (no != NULL) {
		meuGet(duracao, CAMPO_INVALIDO, 5);
	    while (!validaDuracao(duracao)) {
	        printf("%s", CAMPO_INVALIDO);
	        meuGet(duracao, CAMPO_INVALIDO, 5);
	    }
		for (i = 0; i < no->num_chaves; i++) {
			if (strcmp(pk, no->chave[i].pk) == 0) {
				buffer = ARQUIVO + no->chave[i].rrn;
                while (j < 4) {
                    if (*buffer == '@') {
                        j++;
                    }
                    buffer++;
                }
				strncpy(buffer, duracao, 5);
			}
		}
	} else {
		printf(REGISTRO_N_ENCONTRADO);
	}
}

void buscar(Iprimary iprimary, Iwinner *iwinner, Imvp *imvp, int nregistros) {
	char mvp[TAM_MVP], pk[TAM_PRIMARY_KEY], vencedor[TAM_EQUIPE];
	int opcao, i;
	node_Btree *no;

	scanf("%d", &opcao);
	switch (opcao) {
		case 1:
            ignore();
            scanf("%[^\n]", pk);
			printf(NOS_PERCORRIDOS, pk);
			no = (node_Btree *) malloc(sizeof(node_Btree));
			no = buscar_arvore(&iprimary.raiz, pk, 1);
			printf("\n");
			if (no != NULL) {
				for (i = 0; i < no->num_chaves; i++) {
					if (strcmp(no->chave[i].pk, pk) == 0) {
						exibir_registro(no->chave[i].rrn);
					}
				}
			} else {
				printf(REGISTRO_N_ENCONTRADO);
			}
			break;
		case 2:
            ignore();
            scanf("%[^\n]", vencedor);
			buscar_vencedor(&iprimary, iwinner, vencedor, nregistros);
			break;
		case 3:
            ignore();
            scanf("%[^\n]", mvp);
			buscar_mvp(&iprimary, imvp, mvp, nregistros);
			break;
		default:
            ignore();
			printf(OPCAO_INVALIDA);
			break;
	}
}

void preordem(node_Btree **no, int nivel) {
	int i;

	printf("%d - %s", nivel, (*no)->chave[0].pk);
	for (i = 1; i < (*no)->num_chaves; i++) {
		printf(", %s", (*no)->chave[i].pk);
	}
	printf("\n");
	if (!(*no)->folha) {
		for (i = 0; i <= (*no)->num_chaves; i++) {
			preordem(&(*no)->desc[i], nivel + 1);
		}
	}
}

void listar(Iprimary iprimary, Iwinner *iwinner, Imvp *imvp, int nregistros) {
	int i, j, opcao;
	node_Btree *no;

	scanf("%d", &opcao);

	switch (opcao) {
		case 1:
			if (nregistros == 0) {
				printf(ARQUIVO_VAZIO);
			} else {
				preordem(&iprimary.raiz, 1);
				printf("\n");
			}
			break;
		case 2:
			if (nregistros == 0) {
				printf(ARQUIVO_VAZIO);
			} else {
				no = (node_Btree *) malloc(sizeof(node_Btree));
				for (i = 0; i < nregistros; i++) {
					no = buscar_arvore(&iprimary.raiz, iwinner[i].pk, 0);
					for (j = 0; j < no->num_chaves; j++) {
						if (strcmp(iwinner[i].pk, no->chave[j].pk) == 0) {
							exibir_registro(no->chave[j].rrn);
						}
					}
				}
			}
			break;
		case 3:
			if (nregistros == 0) {
				printf(ARQUIVO_VAZIO);
			} else {
				no = (node_Btree *) malloc(sizeof(node_Btree));
				for (i = 0; i < nregistros; i++) {
					no = buscar_arvore(&iprimary.raiz, imvp[i].pk, 0);
					for (j = 0; j < no->num_chaves; j++) {
						if (strcmp(imvp[i].pk, no->chave[j].pk) == 0) {
							exibir_registro(no->chave[j].rrn);
						}
					}
				}
			}
			break;
		default:
            ignore();
			printf(OPCAO_INVALIDA);
			break;
	}
}

void apagar_no(node_Btree **raiz) {
	int i;

	if ((*raiz) != NULL) {
		for (i = 0; i <= (*raiz)->num_chaves; i++) {
			if ((*raiz)->desc[i] != NULL) {
				apagar_no(&(*raiz)->desc[i]);
			}
		}

		free(*raiz);
	}

}

void ignore() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
}
