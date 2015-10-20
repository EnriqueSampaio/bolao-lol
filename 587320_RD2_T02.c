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
/* Compara dois registros do índice secundário de equipe vencedora, para auxílio da função quicksort */
int compareIwinner(const void * a, const void * b);

/* Compara dois registros do índice secundário de mvp, para auxílio da função quicksort */
int compareImvp(const void * a, const void * b);

/* Gera o índice primário na estrutura de Árvore-B */
void criar_iprimary(Iprimary *iprimary, int nregistros, int ordem);

/* Gera o índice secundário de equipe vencedora */
void criar_iwinner(Iwinner iwinner, int nregistros);

/* Gera o índice secundário de MVP */
void criar_iwinner(Imvp imvp, int nregistros);

/* Cadastra nova partida */
void cadastrar(Iprimary *iprimary, Iwinner iwinner, Imvp imvp, int *nregistros);

/* Altera o tempo de duração da partida */
void alterar(Iprimary iprimary);

/* Busca partida */
void buscar(Iprimary iprimary, Iwinner iwinner, Imvp imvp, int nregistros);

/* Lista partida */
void listar(Iprimary iprimary, Iwinner iwinner, Imvp imvp, int nregistros);

/* Apaga nó da Árvore-B (índice primário) */
void apagar_no(Iprimary *iprimary.raiz);

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
void insere(*iprimary, Chave *chave) {
	node_Btree *novo;

	if (iprimary->raiz == NULL) {
		novo = (node_Btree *) malloc(sizeof(node_Btree));
		novo.folha = 1;
		novo.num_chaves = 1;
		novo.chaves = (Chave *) malloc((m - 1) * sizeof(Chave));
		novo.desc = (node_Btree**) malloc(m * sizeof(node_Btree*));
		novo.chaves[0] = *chave;
		iprimary->raiz = novo;
	} else {
		//insere aux

		if (chavePromovida != NULL) {
			novo = (node_Btree *) malloc(sizeof(node_Btree));
			novo.folha = 0;
			novo.num_chaves = 1;
			*novo.chaves = (Chave *) malloc((m - 1) * sizeof(Chave));
			novo.desc = (node_Btree**) malloc(m * sizeof(node_Btree*));
			novo.chaves[0] = chavePromovida;
			novo.desc[0] = raiz;
			novo.desc[1] = filhoDireito;
			raiz = novo;
		}
	}
}

int compareIwinner(const void * a, const void * b) {
    Iwinner *elem1 = (Iwinner*)a;
	Iwinner *elem2 = (Iwinner*)b;

    return strcmp(elem1->vencedor, elem2->vencedor);
}

int compareImvp(const void * a, const void * b) {
    Imvp *elem1 = (Imvp*)a;
	Imvp *elem2 = (Imvp*)b;

    return strcmp(elem1->mvp, elem2->mvp);
}

void criar_iprimary(Iprimary *iprimary, int nregistros, int ordem) {
	char aux[TAM_REGISTRO + 1], lixo[40];
	Chave aux;
	int i, j;
	Partida p;

	for (i = 0, j = 0; j < nregistros; i + 192, j++) {
		*aux = ARQUIVO + i;
		sscanf(aux, "%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c", aux.pk);
		aux.rrn = i;
	}
}

void criar_iwinner(Iwinner iwinner, int nregistros) {
	char aux[TAM_REGISTRO + 1], lixo[40];
	int i, j;
	Partida p;

	for (i = 0, j = 0; j < nregistros; i + 192, j++) {
		*aux = ARQUIVO + i;
		sscanf(aux, "%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c", iwinner[j].pk, lixo, lixo, lixo, lixo, iwinner[j].vencedor, lixo, lixo, lixo);
	}
	qsort(iwinner, nregistros, sizeof(Iwinner), compareIwinner);
}

void criar_imvp(Imvp imvp, int nregistros) {
	char aux[TAM_REGISTRO + 1], lixo[40];
	int i, j;
	Partida p;

	for (i = 0, j = 0; j < nregistros; i + 192, j++) {
		*aux = ARQUIVO + i;
		sscanf(aux, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", p.pk, lixo, lixo, lixo, lixo, lixo, lixo, lixo, p.mvp);
		strcpy(imvp[j].mvp, p.mvp);
		strcpy(imvp[j].pk, p.codigo);
	}
	qsort(imvp, nregistros, sizeof(Imvp), compareImvp);
}
