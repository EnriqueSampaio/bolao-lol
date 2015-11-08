/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
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
#define MAX_REGISTROS 5000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n\n"
#define REGISTRO_N_ENCONTRADO "Registro nao encontrado!\n\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ERRO_TABELA_CHEIA "ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"

#define POS_OCUPADA 	"[%d] Ocupado: %s\n"
#define POS_LIVRE 		"[%d] Livre\n"
#define POS_REMOVIDA 	"[%d] Removido\n"

/* Estado das posições da tabela hash */
#define LIVRE 0
#define OCUPADO 1
#define REMOVIDO 2

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
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct {
	int estado;
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
  int tam;
  Chave *v;
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

/* Realiza o reespalhamento linear em caso de colisão na tabela Hash. Caso a tabela esteja cheia, retorna -1 */
int reespalhamento_linear(Hashtable tabela, int pos);

/* Cria a tabela Hash*/
void criar_tabela(Hashtable *tabela, int tam);

/* Insere novo elemento na tabela Hash e retorna o número de colisões, -1 caso a tabela esteja cheia ou -2 se o elemento for repetido */
int inserir_tabela(Hashtable *tabela, char pk[], int rrn);

/* Carrega a tabela Hash com os dados da string que simula o arquivo de dados */
void carregar_tabela(Hashtable *tabela);

/* Função própria para receber dados até encontrar uma quebra de linha, exibindo um erro caso a entrada seja maior que o especificado */
void meu_get(char string[], char erro[], int limite);

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
	int i;

	tabela->tam = tam;
	tabela->v = (Chave *) malloc(tabela->tam * sizeof(Chave));

	for (i = 0; i < tabela->tam; i++) {
		tabela->v[i].estado = LIVRE;
	}
}

/* Realiza o reespalhamento linear em caso de colisão na tabela Hash. Caso a tabela esteja cheia, retorna -1, e caso o elemento seja repetido, retorna -2 */
int reespalhamento_linear(Hashtable tabela, int pos, char pk[]) {
	int i;

	if (pos == tabela.tam - 1) {
		i = 0;
	} else {
		i = pos + 1;
	}

	while (tabela.v[i].estado == OCUPADO) {
		if (i == tabela.tam - 1) {
			i = 0;
		} else if (i == pos) {
			return -1;
		} else if (strcmp(pk, tabela.v[i].pk) == 0) {
			return -2;
		} else {
			i++;
		}
	}

	return i;
}

/* Gera a hash */
int gerar_hash(int tam, char pk[]) {
	return (pk[0] * 1 + pk[1] * 2 + pk[2] * 3 + pk[3] * 4 + pk[4] * 5 + pk[5] * 6 + pk[6] * 7 + pk[7] * 8) % tam;
}

/* Insere novo elemento na tabela Hash e retorna o número de colisões, -1 caso a tabela esteja cheia ou -2 se o elemento for repetido */
int inserir_tabela(Hashtable *tabela, char pk[], int rrn) {
	int pos;

	pos = gerar_hash(tabela->tam, pk);

	if (tabela->v[pos].estado == LIVRE || tabela->v[pos].estado == REMOVIDO) {
		tabela->v[pos].estado = OCUPADO;
		tabela->v[pos].rrn = rrn;
		strcpy(tabela->v[pos].pk, pk);
	} else {
		if (strcmp(pk, tabela->v[pos].pk) == 0) {
			return -2;
		} else {
			pos = reespalhamento_linear(*tabela, pos);
			if (pos != -1) {
				tabela->v[pos].estado = OCUPADO;
				tabela->v[pos].rrn = rrn;
				strcpy(tabela->v[pos].pk, pk);
			} else {
				return -1;
			}
		}
	}

	return 1;
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

/* Função própria para receber dados até encontrar uma quebra de linha, exibindo um erro caso a entrada seja maior que o especificado */
void meu_get(char string[], char erro[], int limite) {
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

/* Valida se a data foi inserida corretamente */
int validar_data(const char string[]) {
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

/* Valida se a duração foi inserida corretamente */
int validar_duracao(const char string[]) {
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

/* Cadastra uma nova partida */
void cadastrar(Hashtable *tabela) {
	Partida p;
    char registro[TAM_REGISTRO + 1], *buffer;
	Chave chave;
	int i, j;

    registro[0] = '\0';

    meu_get(p.equipe_azul, CAMPO_INVALIDO, TAM_EQUIPE - 1);
    meu_get(p.equipe_vermelha, CAMPO_INVALIDO, TAM_EQUIPE - 1);

    meu_get(p.data_partida, CAMPO_INVALIDO, TAM_DATA - 1);
    while (!validar_data(p.data_partida)) {
        printf("%s", CAMPO_INVALIDO);
        meu_get(p.data_partida, CAMPO_INVALIDO, TAM_DATA - 1);
    }

    meu_get(p.duracao, CAMPO_INVALIDO, TAM_DURACAO - 1);
    while (!validar_duracao(p.duracao)) {
        printf("%s", CAMPO_INVALIDO);
        meu_get(p.duracao, CAMPO_INVALIDO, TAM_DURACAO - 1);
    }

    meu_get(p.vencedor, CAMPO_INVALIDO, TAM_EQUIPE - 1);

    while (strcmp(p.vencedor, p.equipe_azul) != 0 && strcmp(p.vencedor, p.equipe_vermelha) != 0) {
        printf("%s", CAMPO_INVALIDO);
        meu_get(p.vencedor, CAMPO_INVALIDO, TAM_EQUIPE - 1);
    }

    meu_get(p.placar1, CAMPO_INVALIDO, TAM_PLACAR - 1);

    while (!validar_placar(p.placar1)) {
        printf("%s", CAMPO_INVALIDO);
        meu_get(p.placar1, CAMPO_INVALIDO, TAM_PLACAR - 1);
    }

    meu_get(p.placar2, CAMPO_INVALIDO, TAM_PLACAR - 1);

    while (!validar_placar(p.placar2)) {
        printf("%s", CAMPO_INVALIDO);
        meu_get(p.placar2, CAMPO_INVALIDO, TAM_PLACAR - 1);
    }

    meu_get(p.mvp, CAMPO_INVALIDO, TAM_MVP - 1);
    gerar_pk(&p);

    if(verificar_chave(iprimary, p.pk)) {
        criar_registro(&p, registro);
		buffer = ARQUIVO + strlen(ARQUIVO);
		sprintf(buffer, "%s", registro);
		strcpy(chave.pk, p.pk);
		chave.rrn = *nregistros * 192;
		insere(iprimary, &chave);
    } else {
        printf(ERRO_PK_REPETIDA, p.pk);
    }
}

/* Altera a duração de uma partida */
void alterar(Hashtable tabela) {
	//IMPLEMENTAR
}

/* Busca por uma partida na tabela Hash através de uma chave primária recebida */
void buscar(Hashtable tabela) {
	char pk[TAM_PRIMARY_KEY];
	int i, pos;

	scanf("%[^\n]", pk);
	getchar();

	i = pos = gerar_hash(tabela.tam, pk);

	if (strcmp(pk, tabela.v[i].pk) == 0) {
		exibir_registro(tabela.v[i].rrn);
	} else {
		if (i == tabela.tam - 1) {
			i = 0;
		} else {
			i++;
		}

		while (strcmp(pk, tabela.v[i].pk) != 0) {
			if (i == tabela.tam - 1) {
				i = 0;
			} else if (i == pos) {
				printf(REGISTRO_N_ENCONTRADO);
				return;
			} else {
				i++;
			}
		}

		exibir_registro(tabela.v[i].rrn);
	}

}

/* Remove uma partida, marcando como removida na tabela Hash e no arquivo de dados */
void remover(Hashtable *tabela) {
	char *buffer, pk[TAM_PRIMARY_KEY];
	int i, pos;

	scanf("%[^\n]", pk);
	getchar();

	i = pos = gerar_hash(tabela->tam, pk);

	if (strcmp(pk, tabela->v[i].pk) == 0) {
		buffer = ARQUIVO + tabela->v[i].rrn;
		strcpy(buffer, "*|");
	} else {
		if (i == tabela->tam - 1) {
			i = 0;
		} else {
			i++;
		}

		while (strcmp(pk, tabela->v[i].pk) != 0) {
			if (i == tabela->tam - 1) {
				i = 0;
			} else if (i == pos) {
				printf(REGISTRO_N_ENCONTRADO);
				return;
			} else {
				i++;
			}
		}

		buffer = ARQUIVO + tabela->v[i].rrn;
		strcpy(buffer, "*|");
	}
}

/* Imprime os dados da tabela Hash */
void imprimir_tabela(Hashtable tabela) {
	int i;

	for (i = 0; i < tabela.tam; i++) {
		if (tabela.v[i].estado == LIVRE) {
			printf(POS_LIVRE, i);
		} else if (tabela.v[i].estado == OCUPADO) {
			printf(POS_OCUPADA, i, tabela.v[i].pk);
		} else {
			printf(POS_REMOVIDA, i);
		}
	}
}

/* Libera a memória ocupada pela tabela Hash antes de finalizar o programa */
void liberar_tabela(Hashtable *tabela) {
	free(tabela->v);
}
