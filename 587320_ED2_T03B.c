/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03B - Hashing com encadeamento
 *
 * RA:
 * Aluno:
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
#define MAX_REGISTROS 5000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n\n"
#define REGISTRO_N_ENCONTRADO "Registro nao encontrado!\n\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"

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

/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave Chave;
struct chave {
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	Chave *prox;
};

/* Estrutura da Tabela Hash */
typedef struct {
  int tam;
  Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];


/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Descarta o que estiver no buffer de entrada */
void ignore();

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o jogador */
void exibir_registro(int rrn);

/* Recupera um registro do arquivo de dados, dado um RRN */
Partida recuperar_registro(int rrn);

/* Retorna o número primo maior e mais próximo ou igual a tam */
int prox_primo(int tam);

/* Cria a tabela Hash*/
void criar_tabela(Hashtable *tabela, int tam);

/* Gera a hash */
int gerar_hash(int tam, char pk[]);

/* Insere novo elemento na tabela Hash e retorna 1 em caso de sucesso ou 0 se o elemento for repetido */
int inserir_tabela(Hashtable *tabela, char pk[], int rrn);

/* Carrega a tabela Hash com os dados da string que simula o arquivo de dados */
void carregar_tabela(Hashtable *tabela);

/* Valida se a data foi inserida corretamente */
int validar_data(const char string[]);

/* Valida se a duração foi inserida corretamente */
int validar_duracao(const char string[]);

/* Valida se o placar foi inserido corretamente */
int validar_placar(const char string[]);

/* Gera a chave primária do registro */
void gerar_pk(Partida *p);

/* Cria um novo registro com os dados recebidos para ser inserido no arquivo de dados */
void criar_registro(Partida *p, char registro[]);

/* Cadastra uma nova partida */
void cadastrar(Hashtable *tabela);

/* Auxiliar para a busca utilizado na função buscar, alterar e remover, retorna o rrn do registro ou -1 se não existir */
int buscar_aux(Hashtable *tabela, char pk[], int hash, int remover);

/* Altera a duração de uma partida */
void alterar(Hashtable tabela);

/* Busca por uma partida na tabela Hash através de uma chave primária recebida */
void buscar(Hashtable tabela);

/* Remove uma partida, marcando como removida na tabela Hash e no arquivo de dados */
void remover(Hashtable *tabela);

/* Imprime os dados da tabela Hash */
void imprimir_tabela(Hashtable tabela);

/* Função recursiva auxiliar da função liberar_tabela, libera recursivamente todos nós da lista encadeada */
void liberar_aux(Chave *no);

/* Libera a memória ocupada pela tabela Hash antes de finalizar o programa */
void liberar_tabela(Hashtable *tabela);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main() {

	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) carregar_arquivo();



	/* Tabela Hash */
	int tam;
	scanf("%d", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo) carregar_tabela(&tabela);



	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
		scanf("%d", &opcao);
		switch(opcao) {

		case 1:
			getchar();
			cadastrar(&tabela);
			break;
		case 2:
			getchar();
			alterar(tabela);
			break;
		case 3:
			getchar();
			buscar(tabela);
			break;
		case 4:
			remover(&tabela);
			break;
		case 5:
			imprimir_tabela(tabela);
			break;
		case 6:
			liberar_tabela(&tabela);
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

/* Descarta o que estiver no buffer de entrada */
void ignore() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
}

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
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

/* Recupera um registro do arquivo de dados, dado um RRN */
Partida recuperar_registro(int rrn) {
	char *buffer;
	Partida p;

	buffer = ARQUIVO + rrn;
	sscanf(buffer, "%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c%[^@]%*c", p.pk, p.equipe_azul, p.equipe_vermelha, p.data_partida, p.duracao, p.vencedor, p.placar1, p.placar2, p.mvp);
	return p;
}

/* Retorna o número primo maior e mais próximo ou igual a tam */
int prox_primo(int tam) {
	int i, j, flag = 1;

	if (tam == 0 || tam == 1) {
		return tam;
	}

	for (i = tam; flag ; i++) {
		flag = 0;
		for (j = 2; j < i; j++) {
			if (i % j == 0) {
				flag = 1;
			}
		}
	}

	return (i - 1);
}

/* Cria a tabela Hash*/
void criar_tabela(Hashtable *tabela, int tam) {
	int i;

	tabela->tam = tam;
	tabela->v = (Chave **) malloc(tabela->tam * sizeof(Chave *));

	for (i = 0; i < tabela->tam; i++) {
		tabela->v[i] = NULL;
	}
}

/* Gera a hash */
int gerar_hash(int tam, char pk[]) {
	return (pk[0] * 1 + pk[1] * 2 + pk[2] * 3 + pk[3] * 4 + pk[4] * 5 + pk[5] * 6 + pk[6] * 7 + pk[7] * 8) % tam;
}

/* Insere novo elemento na tabela Hash e retorna 1 em caso de sucesso ou 0 se o elemento for repetido */
int inserir_tabela(Hashtable *tabela, char pk[], int rrn) {
	Chave *aux, *novo;
	int hash = gerar_hash(tabela->tam, pk);

	aux = tabela->v[hash];

	if (aux == NULL) {
		novo = (Chave *) malloc(sizeof(Chave));
		novo->rrn = rrn;
		strcpy(novo->pk, pk);
		novo->prox = NULL;
		tabela->v[hash] = novo;
		return 1;
	} else if (!strcmp(pk, aux->pk)) {
		return 0;
	}

	while (aux->prox != NULL && strcmp(pk, aux->prox->pk) > 0) {
		aux = aux->prox;
	}

	if (strcmp(pk, aux->pk) < 0) {
		novo = (Chave *) malloc(sizeof(Chave));
		novo->rrn = rrn;
		strcpy(novo->pk, pk);
		novo->prox = aux;
		tabela->v[hash] = novo;
		return 1;
	} else if (aux->prox == NULL || strcmp(pk, aux->prox->pk) < 0) {
		novo = (Chave *) malloc(sizeof(Chave));
		novo->rrn = rrn;
		strcpy(novo->pk, pk);
		novo->prox = aux->prox;
		aux->prox = novo;
		return 1;
	} else {
		return 0;
	}
}

/* Carrega a tabela Hash com os dados da string que simula o arquivo de dados */
void carregar_tabela(Hashtable *tabela) {
	char *buffer, pk[TAM_PRIMARY_KEY];
	int i;

	for (i = 0; i < strlen(ARQUIVO); i+= 192) {
		buffer = ARQUIVO + i;
		sscanf(buffer, "%[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c%*[^@]%*c", pk);
		inserir_tabela(tabela, pk, i);
	}
}

/* Valida se a data foi inserida corretamente */
int validar_data(const char string[]) {
    char *dia, *mes, *ano, *data;
    long diaNum, mesNum, anoNum;

    data = (char *) malloc(TAM_DATA * sizeof(char));
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

/* Valida se a duração foi inserida corretamente */
int validar_duracao(const char string[]) {
    char *minuto, *segundo, *duracao;

    duracao = (char *) malloc(TAM_DURACAO * sizeof(char));
    strcpy(duracao, string);

    minuto = strtok(duracao, ":");
    segundo = strtok(NULL, "\0");

    if(strlen(minuto) != 2 || strlen(segundo) != 2){
        return 0;
    } else if (minuto[0] < '0' || minuto[0] > '9' || minuto[1] < '0' || minuto[1] > '9' || segundo[0] < '0' || segundo[0] > '5' || segundo[1] < '0' || segundo[1] > '9') {
		return 0;
	}

    return 1;
}

/* Valida se o placar foi inserido corretamente */
int validar_placar(const char string[]) {
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

/* Gera a chave primária do registro */
void gerar_pk(Partida *p) {
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

/* Cria um novo registro com os dados recebidos para ser inserido no arquivo de dados */
void criar_registro(Partida *p, char registro[]) {
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

void cadastrar(Hashtable *tabela) {
	Partida p;
    char registro[TAM_REGISTRO + 1], entrada[TAM_EQUIPE], *buffer;

    registro[0] = '\0';

    scanf("%[^\n]", entrada);
	ignore();
	while (strlen(entrada) > TAM_EQUIPE - 1) {
		printf(CAMPO_INVALIDO);
		scanf("%[^\n]", entrada);
		ignore();
	}
	strcpy(p.equipe_azul, entrada);

	scanf("%[^\n]", entrada);
	ignore();
	while (strlen(entrada) > TAM_EQUIPE - 1) {
		printf(CAMPO_INVALIDO);
		scanf("%[^\n]", entrada);
		ignore();
	}
	strcpy(p.equipe_vermelha, entrada);

	scanf("%[^\n]", entrada);
	ignore();
	while (strlen(entrada) > TAM_DATA - 1 || !validar_data(entrada)) {
		printf(CAMPO_INVALIDO);
		scanf("%[^\n]", entrada);
		ignore();
	}
	strcpy(p.data_partida, entrada);

	scanf("%[^\n]", entrada);
	ignore();
	while (strlen(entrada) > TAM_DURACAO - 1 || !validar_duracao(entrada)) {
		printf(CAMPO_INVALIDO);
		scanf("%[^\n]", entrada);
		ignore();
	}
	strcpy(p.duracao, entrada);

	scanf("%[^\n]", entrada);
	ignore();
	while (strlen(entrada) > TAM_EQUIPE - 1 || (strcmp(entrada, p.equipe_azul) != 0 && strcmp(entrada, p.equipe_vermelha))) {
		printf(CAMPO_INVALIDO);
		scanf("%[^\n]", entrada);
		ignore();
	}
	strcpy(p.vencedor, entrada);

	scanf("%[^\n]", entrada);
	ignore();
	while (strlen(entrada) > TAM_PLACAR - 1 || !validar_placar(entrada)) {
		printf(CAMPO_INVALIDO);
		scanf("%[^\n]", entrada);
		ignore();
	}
	strcpy(p.placar1, entrada);

	scanf("%[^\n]", entrada);
	ignore();
	while (strlen(entrada) > TAM_PLACAR - 1 || !validar_placar(entrada)) {
		printf(CAMPO_INVALIDO);
		scanf("%[^\n]", entrada);
		ignore();
	}
	strcpy(p.placar2, entrada);

	scanf("%[^\n]", entrada);
	ignore();
	while (strlen(entrada) > TAM_EQUIPE - 1) {
		printf(CAMPO_INVALIDO);
		scanf("%[^\n]", entrada);
		ignore();
	}
	strcpy(p.mvp, entrada);
    gerar_pk(&p);

    if(inserir_tabela(tabela, p.pk, strlen(ARQUIVO))) {
        criar_registro(&p, registro);
		buffer = ARQUIVO + strlen(ARQUIVO);
		sprintf(buffer, "%s", registro);
		printf(REGISTRO_INSERIDO, p.pk);
    } else {
        printf(ERRO_PK_REPETIDA, p.pk);
    }
}

/* Auxiliar para a busca utilizado na função buscar, alterar e remover, retorna o rrn do registro ou -1 se não existir */
int buscar_aux(Hashtable *tabela, char pk[], int hash, int remover) {
	Chave *aux, *ant;
	int rrn;

	ant = aux = tabela->v[hash];

	if (aux == NULL) {
		return -1;
	} else if (aux->prox == NULL && !strcmp(aux->pk, pk)) {
		rrn = aux->rrn;
		if (remover) {
			tabela->v[hash] = NULL;
			free(aux);
		}
		return rrn;
	}

	while (aux->prox != NULL && strcmp(pk, aux->pk)) {
		ant = aux;
		aux = aux->prox;
	}

	if (!strcmp(pk, aux->pk)) {
		rrn = aux->rrn;
		if (remover) {
			ant->prox = aux->prox;
			free(aux);
		}
		return rrn;
	}

	return -1;
}

/* Altera a duração de uma partida */
void alterar(Hashtable tabela) {
	char *buffer, duracao[TAM_DURACAO], entrada[TAM_EQUIPE], pk[TAM_PRIMARY_KEY];
	int i = 0, hash, rrn;

	scanf("%[^\n]", pk);
	ignore();

	hash = gerar_hash(tabela.tam, pk);
	rrn = buscar_aux(&tabela, pk, hash, 0);

	if (rrn == -1) {
		printf(REGISTRO_N_ENCONTRADO);
	} else {
		scanf("%[^\n]", entrada);
		ignore();
		while (strlen(entrada) > TAM_DURACAO - 1 || !validar_duracao(entrada)) {
			printf(CAMPO_INVALIDO);
			scanf("%[^\n]", entrada);
			ignore();
		}
		strcpy(duracao, entrada);
		buffer = ARQUIVO + rrn;
        while (i < 4) {
            if (*buffer == '@') {
                i++;
            }
            buffer++;
        }
		strncpy(buffer, duracao, TAM_DURACAO - 1);
	}
}

/* Busca por uma partida na tabela Hash através de uma chave primária recebida */
void buscar(Hashtable tabela) {
	char pk[TAM_PRIMARY_KEY];
	int hash, rrn;

	scanf("%[^\n]", pk);
	ignore();

	hash = gerar_hash(tabela.tam, pk);
	rrn = buscar_aux(&tabela, pk, hash, 0);

	if (rrn == -1) {
		printf(REGISTRO_N_ENCONTRADO);
	} else {
		exibir_registro(rrn);
	}
}

/* Remove uma partida, marcando como removida na tabela Hash e no arquivo de dados */
void remover(Hashtable *tabela) {
	char *buffer, pk[TAM_PRIMARY_KEY];
	int hash, rrn;

	getchar();
	scanf("%[^\n]", pk);
	ignore();

	hash = gerar_hash(tabela->tam, pk);
	rrn = buscar_aux(tabela, pk, hash, 1);

	if (rrn == -1) {
		printf(REGISTRO_N_ENCONTRADO);
	} else {
		buffer = ARQUIVO + rrn;
		strncpy(buffer, "*|", 2);
	}
}

/* Imprime os dados da tabela Hash */
void imprimir_tabela(Hashtable tabela) {
	Chave *aux;
	int i;

	for (i = 0; i < tabela.tam; i++) {
		aux = tabela.v[i];
		printf("[%d]", i);
		while (aux != NULL) {
			printf(" %s", aux->pk);
			aux = aux->prox;
		}
		printf("\n");
	}
}

/* Função recursiva auxiliar da função liberar_tabela, libera recursivamente todos nós da lista encadeada */
void liberar_aux(Chave *no) {
	if (no == NULL) {
		return;
	} else {
		liberar_aux(no->prox);
		free(no);
	}
}

/* Libera a memória ocupada pela tabela Hash antes de finalizar o programa */
void liberar_tabela(Hashtable *tabela) {
	int i;

	for (i = 0; i < tabela->tam; i++) {
		liberar_aux(tabela->v[i]);
	}
}
