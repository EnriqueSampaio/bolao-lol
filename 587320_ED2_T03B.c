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

/* Carrega a tabela Hash com os dados da string que simula o arquivo de dados */
void carregar_tabela(Hashtable *tabela);

/* Cadastra uma nova partida */
void cadastrar(Hashtable *tabela);

/* Altera a duração de uma partida */
void alterar(Hashtable tabela);

/* Busca por uma partida na tabela Hash através de uma chave primária recebida */
void buscar(Hashtable tabela);

/* Remove uma partida, marcando como removida na tabela Hash e no arquivo de dados */
void remover(Hashtable *tabela);

/* Imprime os dados da tabela Hash */
void imprimir_tabela(Hashtable tabela);

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
	//IMPLEMENTAR
}

/* Carrega a tabela Hash com os dados da string que simula o arquivo de dados */
void carregar_tabela(Hashtable *tabela) {
	//IMPLEMENTAR
}

/* Cadastra uma nova partida */
void cadastrar(Hashtable *tabela) {
	// IMPLEMENTAR
}

/* Altera a duração de uma partida */
void alterar(Hashtable tabela) {
	// IMPLEMENTAR
}

/* Busca por uma partida na tabela Hash através de uma chave primária recebida */
void buscar(Hashtable tabela) {
	// IMPLEMENTAR
}

/* Remove uma partida, marcando como removida na tabela Hash e no arquivo de dados */
void remover(Hashtable *tabela) {
	// IMPLEMENTAR
}

/* Imprime os dados da tabela Hash */
void imprimir_tabela(Hashtable tabela) {
	// IMPLEMENTAR
}

/* Libera a memória ocupada pela tabela Hash antes de finalizar o programa */
void liberar_tabela(Hashtable *tabela) {
	// IMPLEMENTAR
}
