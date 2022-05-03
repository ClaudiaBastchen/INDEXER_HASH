/* CLAUDIA & RAFAEL*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#define NPAL 64 /*DIMENSAO MAX DE CADA PALAVRA*/
#define NTAB 997 /*DIMENSAO TABELA DISPERSÃO*/

#ifndef __estruturas__
#define __estruturas__

//TIPO QUE REPRESENTA CADA PALAVRA
typedef struct nodo { 
	char *key; 
	int value; 
	struct nodo *next;//TRATAMENTO DE COLISÕES
} node;

//TIPO QUE REPRESENTA A TABELA DE DISPERSÃO
typedef struct {
	int tamanho;
	int tamanho_atual;
	node **list; 
} hashTable;

//LISTA QUANTIDADE DE TERMOS NA LISTA
typedef struct listaTermo {
	char * value;
	int nDocs; // QTD DOC EM QUE ESTE TERMO ESTA PRSENTE
	struct listaTermo *next;
} termosList;

//LISTA ARQUIVOS
typedef struct listaArquivo {
	char * value;
	hashTable * table; // TABELA HASH RESPONSÁVEL POR CONTAR AS PALAVRAS DO ARQUIVO
	int countPalavras; // NÚMERO DE PALAVRAS DO ARQUIVO 
	struct listaArquivo *next;
} arquivosList;


typedef struct {
	char * value;
	double relevancia;
} arqRelevancia;

#endif

//PRIMEIRA FUNÇÃO RESPONSÁVEL POR CRIAR E INICIALIZAR A TABELA

hashTable* createHashTable (int tamanho) {

	if (tamanho < 1) // CASO TAMANHO FORNECIDO INVÁLIDO
		return NULL;

	hashTable * table = NULL;

	// TABELA HASH ALOCADA
	
	if ( (table = malloc(sizeof(hashTable)) ) == NULL )
		return NULL;

	// PONTEIROS ALOCADO PARA A LISTA ENCADEADA
	if ( (table->list = malloc(sizeof( node * )*tamanho) ) == NULL )
		return NULL;

	// GARANTE QUE TODOS OS ELEMENTOS DE CADA UMA DAS LISTAS SÃO NULOS
	int i;
	for( i = 0; i < tamanho; i++ ) {
		table->list[i] = NULL;
	}

	table->tamanho_atual = 0;
	table->tamanho = tamanho;
	return table;
}

/*this algorithm (k=33) was first reported by dan bernstein many years ago in comp.lang.c.
 another version of this algorithm (now favored by bernstein) uses xor:
 hash(i) = hash(i - 1) * 33 ^ str[i]; the magic of number 33 (why it works better than many
 other constants, prime or not) has never been adequately explained.*/
 
//DJB2 FUNÇÃO HASH
int hashKey(char * key, int tamanho) {
	unsigned long hash = 5381; //http://www.cse.yorku.ca/~oz/hash.html
    int c;
    while (c = *key++)
        hash = ((hash << 5) + hash) + c; /* hash*33+c */

    return hash%tamanho;
}

node * createNode( char * key ) {

	node * novoNodo = NULL;

	// GARANTIA DE ALOCAÇÃO SEM ERROS
	if ( ( novoNodo = malloc(sizeof(node)) ) == NULL )
		return NULL;

	// GARANTIA DE ALOCAÇÃO DE CHAVE SEM ERROS
	if ( ( novoNodo->key = strdup(key) ) == NULL )
		return NULL;

	novoNodo->next = NULL;
	novoNodo->value = 1;
	return novoNodo;
}

node * searchHashTable( hashTable* table, char * key ) {

	// SE A TABELA FOR NULA
	if (table == NULL)
		return NULL;

	// RETORNO DA POSIÇÃO DISPONÍVEL DA TABELA
	int hashPos;
	hashPos = hashKey(key, table->tamanho);

	// VALOR INICIAL DA LISTA NÃO NULO, POR FIM BUSCAMOS SE JA EXISTE ALGO
	node *nodoBusca = table->list[hashPos];
	node *resultante = NULL;
	while (nodoBusca != NULL) {
		// VALOR ENCONTRADO
		if (strcmp(nodoBusca->key, key) == 0) {
			resultante = nodoBusca;
			break;
		}
		nodoBusca = nodoBusca->next;
	}

	return resultante;
}

void incrementHashTable( hashTable* table, char * key ) {

	//SE A TABELA FOR NULA
	if (table == NULL)
		return;

	// RETORNO DA POSIÇÃO DISPONÍVEL NA TABELA
	int hashPos;
	hashPos = hashKey(key, table->tamanho);

	// VALOR INICIAL NÃO NULO. FAZ BUSCA SE JA EXSTE
	node * ultimo = NULL;
	node * nodoBusca = table->list[hashPos];
	node * resultante = NULL;
	while (nodoBusca != NULL) {
		
		// VALOR ENCONTRADO
		if (strcmp(nodoBusca->key, key) == 0) {
			resultante = nodoBusca;
			break;
		}
		ultimo = nodoBusca;
		nodoBusca = nodoBusca->next;
	}

	if (resultante != NULL) {
		// ENCONTRADO UM NODO, INCREMENTA UM NO VALOR DO NODO
		(resultante-> value++);
		return;
	} else {

		// NÃO ENCONTRADO VALOR PARA RESULTANTE, CRIA UM NOVO
		resultante = createNode(key); // .
		(table->tamanho_atual)++;

		// SELECIONA PARA ONDE VAI O NOVO NODO
		if (table->list[hashPos] == NULL) {
			table->list[hashPos] = resultante;
		} else {
			// ULTIMO DEVE EXISTIR, ENTROU NO WHILE AO MENOS UMA VEZ
			ultimo->next = resultante;
		}
		return;
	}
}

//FAZ A LEITURA CARACTER POR CARACTER PARA FORMAR A STRING
char * readWord(FILE * fp, bool * state) {
	char c;
	int i = 0;
	char * word = (char *) malloc( sizeof(char) * NPAL );
	while ( ( (c = fgetc(fp)) != EOF ) ) {
			if (c == ' ' || c == '\n'|| c == '.' || c == ',' || c == '?' || c == '!' || c == '\r' || c == '\t' ) {
			// LETRAS DE SAÍDA
			if (*state == true) {  
			// CASO ESTEJA NO MEIO DA PALAVRE
				if (i == 0) {
					// CASO ARQUIVO INICIE COM ESPAÇO
					continue;
				}

				*state = false;
				word[i++] = '\0';
				return word;
			}

		} else {
			// LETRAS DE ENTRADA
			if (c == '\'' || c == '\"' || c == '*' ) {
				// RECONHECE E CONTINUA
				continue;
			}

			word[i] = c;
			i++;
			if (*state == false) {
				*state = true;

/*while ( ( (c = fgetc(fp)) != EOF ) ) {
		if (isalpha(c));
		break;};
	if (c == EOF)
        return 0;
    else
    	word[i++] = c; //PRIMEIRA LETRA JA FOI CAPTURADA
    	
    //LE OS PROXIMOS CARACTERS QUE SÃO LETRAS
    while (i < NPAL -1 && (c = fgetc(fp)) != EOF && isalpha(c)) word[i++] = c;
     word[i] = '\0';
     return word;*/
		}	
	}	
} 
 
    //FIM DO ARQUIVO
	  if (i > 0)
		return word;
	return NULL;
}

//PROCURA UM ELEMENTO NA TABELA HASH
node** getAllElementsFromHashTable(hashTable* table) {
	node **list;
	if ( (list = malloc( sizeof(node *) * table->tamanho_atual )) == NULL )
		return NULL;

	int i = 0;
	int j = 0;
	node * nodo = NULL;
	while (i < table->tamanho) {
		nodo = table->list[i];
		i++;


		if (nodo == NULL) {
			continue;
		}


		// NODE ENCONTRADO, SEGUE SEQUENCIA
		while (nodo != NULL) {
			list[j] = nodo;
			j++;
			nodo = nodo->next;
		}
	}

	return list;
}

int node_comp(const void* p1, const void* p2) {
	int l = (*((node **)p1))->value;
	int r = (*((node **)p2))->value;
	return r - l;
}

int doc_rel_comp(const void* p1, const void* p2) {
	double l = (*((arqRelevancia **)p1))->relevancia;
	double r = (*((arqRelevancia **)p2))->relevancia;
	if (l > r)
		return -1;
	else
		return 1;
}

termosList* createTermosList(char* value) {
	termosList * novoString = NULL;

	// ALOCAÇÃO DSEM ERROS
	if ( ( novoString = malloc(sizeof(termosList)) ) == NULL )
		return NULL;

	// CHAVE SEM ERROS 
		if ( ( novoString->value = strdup(value) ) == NULL )
		return NULL;

	novoString->nDocs = 0;
	novoString->next = NULL;
	return novoString;
}

//CRIA UMA LISTA COM OS ARQUIVOS PASSADOS COMO PARAMETRO PARA POSTERIOR CALCULO DA RELEVANCIA
arquivosList* createArquivosList(char* value) {
	arquivosList * novoString = NULL;

	// ALOCAÇÃO SEM ERROS
	if ( ( novoString = malloc(sizeof(arquivosList)) ) == NULL )
		return NULL;

	// ALOCAÇÃO SEM ERROS
	if ( ( novoString->table = malloc(sizeof(hashTable)) ) == NULL )
		return NULL;

	// CHAVE SEM ERROS
	if ( ( novoString->value = strdup(value) ) == NULL )
		return NULL;

	novoString->countPalavras = 0;
	novoString->next = NULL;
	return novoString;
}

//CRIA UM "DOCUMENTO" PARA GUARDAR AS RELEVANCIAS
arqRelevancia* createarqRelevancia(char* value, double relevancia) {
	arqRelevancia* novoDocRel = NULL;

	// ALOCAÇÃO SEM ERROS
	if ( ( novoDocRel = malloc(sizeof(arqRelevancia)) ) == NULL )
		return NULL;

	// CHAVE SEM ERROS
	if ( ( novoDocRel->value = strdup(value) ) == NULL )
		return NULL;

	novoDocRel->relevancia = relevancia;
	return novoDocRel;

}

//INCREMENTA PALAVRA
int calcFreq (FILE * fp, hashTable * table) {
	bool state = true; // E se o arquivo de texto começar com espaço? Já resolvido dentro da função readWord.
	int count = 0;
	char * palavra;
	while ( (palavra = readWord(fp, &state)) != NULL ) {
		incrementHashTable(table, palavra);
		free(palavra);
		count++;
	}
	return count;
}

int main( int argc, char **argv ) {
    /*
    argc = 4;
    argv[1] = "--freq";
    argv[2] = "10";
    argv[3] = "103.txt";
     */
	/* 
    argc = 4;
    argv[1] = "--freq-word";
    argv[2] = "the";
    argv[3] = "103.txt";
   */
    /* 
    argc = 6;
    argv[1] = "--search";
    argv[2] = "APOCOLOCYNTOSIS";
    argv[3] = "103.txt";
    argv[4] = "10136.txt";
    argv[5] = "1mb.txt";
    */



	if (argc < 2) {
		printf("ERRO\n");
		return 0;
	}

	if ( strcmp(argv[1], "--freq") == 0 ) {
		if (argc != 4) {
			printf("ERRO\n");
			return 0;
		}

		int nPalavras = atoi(argv[2]);
		char * nomeArquivo = argv[3];

		FILE * fp;
		if ( (fp = fopen(nomeArquivo, "r")) == NULL ) {
			printf("Arquivo inválido.\n");
			return 0;
		}

		hashTable * table = createHashTable( NTAB); // NTAB É O TAMANHO DA TABELA DE DISPERSÃO (HASH)

		calcFreq(fp, table);

		node** lista = getAllElementsFromHashTable(table);
		qsort(lista, table->tamanho_atual, sizeof(node *), node_comp);

		// PALAVRAS LISTADAS NAO PASSARÁ O LIMITE DO VETOR.
		int nValores = nPalavras;
		if (nValores > table->tamanho_atual)
			nValores = table->tamanho_atual;

		for (int i = 0; i < nValores; i++) {
			printf("%s : %d\n", lista[i]->key, lista[i]->value);
		}
		printf("\n\n");

		printf("%d entradas foram listadas.\n", nValores);

	} else if ( strcmp(argv[1], "--freq-word") == 0 ) {
		if (argc != 4) {
			printf("ERRO\n");
			return 0;
		}

		char * palavraRef = argv[2];
		char * nomeArquivo = argv[3];

		FILE * fp;
		if ( (fp = fopen(nomeArquivo, "r")) == NULL ) {
			printf("Arquivo inválido.\n");
			return 0;
		}

		int count = 0;
		bool state = true;
		char * palavra;
		while ( (palavra = readWord(fp, &state)) != NULL ) {
			if (strcmp(palavra, palavraRef) == 0)
				count++;
			free(palavra);
		}

		printf("A palavra %s aparece %d vezes nesse documento.\n", palavraRef, count);

	} else if ( strcmp(argv[1], "--search") == 0 ) {
		if (argc < 4) {
			printf("ERRO\n");
			return 0;
		}

		// Salvando valor original do termo para exibir depois.
		char * termoEnter = strdup(argv[2]);

		// Salvando parâmetros em listas ligadas.
		int totalTermos = 0;
		char *ptr = strtok(argv[2], " ");
		termosList* listaTermos = NULL;
		while (ptr != NULL) {
			termosList* string = createTermosList(ptr);
			string->next = listaTermos;
			listaTermos = string;
			ptr = strtok(NULL, " ");
			totalTermos++;
		}

		int arquivosTotais = argc-3;
		arquivosList* listaArquivos = NULL;
		for (int i = 3; i < argc; i++) {
			arquivosList* string = createArquivosList(argv[i]);
			string->next = listaArquivos;
			listaArquivos = string;
		}


		// Calculo inicial das frequências de cada arquivo em sua hash table, para uso posterior.
		// Contador de palavras totais incluso.
		termosList* stringTermos = NULL; // Será usado depois para iterar pelos termos.
		arquivosList* stringArquivos = listaArquivos;
		while (stringArquivos != NULL) {
			hashTable * table = createHashTable(NTAB);

			FILE * fp;
			if ( (fp = fopen(stringArquivos->value, "r")) == NULL ) {
				printf("INVALIDO.\n");
				return 0;
			}

			int totalPalavras = calcFreq(fp, table);

			stringArquivos->countPalavras = totalPalavras;
			stringArquivos->table = table;

			stringArquivos = stringArquivos->next;
		}


		// Primeiro loop : calcula o número de documentos que X termo aparece.
		stringArquivos = listaArquivos;
		while (stringArquivos != NULL) {
			stringTermos = listaTermos;
			while (stringTermos != NULL) {
				// Se o termo estiver presente neste documento, incrementa um na qtd de documentos que o termo aparece.
				if ( searchHashTable(stringArquivos->table, stringTermos->value) != NULL )
					(stringTermos->nDocs)++;

				stringTermos = stringTermos->next;
			}


			stringArquivos = stringArquivos->next;
		}

		// Vetor para armazenar o par arqRelevancia
		arqRelevancia** listaArq = malloc(sizeof(arqRelevancia *) * arquivosTotais);


		// 2º lOOP : CALCULA O 	QUE FALTA
		stringArquivos = listaArquivos;
		int i = 0;
		while (stringArquivos != NULL) {
			double tfidf = 0;

			stringTermos = listaTermos;
			while (stringTermos != NULL) {
				double tf = 0;
				node * tNode = searchHashTable(stringArquivos->table, stringTermos->value);
				if ( tNode != NULL )
					tf = ((float)(tNode->value))/(stringArquivos->countPalavras);

				double idf = 0;
				if (stringTermos->nDocs != 0) // NÃO PERMITIR DIVISÃO POR 0
					idf = log10( ((float)arquivosTotais)/stringTermos->nDocs);

				tfidf += tf*idf;

				stringTermos = stringTermos->next;
			}

			tfidf = tfidf/totalTermos;
			arqRelevancia * docRel = createarqRelevancia(stringArquivos->value, tfidf);

			listaArq[i] = docRel;
			i++;

			stringArquivos = stringArquivos->next;
		}
        
         
         
		qsort(listaArq, arquivosTotais, sizeof(arqRelevancia *), doc_rel_comp);
		
	

		printf("\n\n");
		printf("TERMO : %s\n", termoEnter);

		for (int i = 0; i < arquivosTotais; i++) {
			printf("%s : %.8f DE RELEVANCIA\n", listaArq[i]->value, listaArq[i]->relevancia);
		}

	} else {
		printf("OPCAO INVALIDA.\n");
	}

	return 0;
}
