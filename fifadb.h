#ifndef FIFADB_H
#define FIFADB_H

#define STR_BUFFER_SIZE 200 // Buffer para strings (usado principalmente para armazenar linhas dos arquivos)
#define NUMBER_OF_PLAYERS 18944 // N�mero de jogadores a serem armazenados

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "conio2.h"
#include <string.h>

// Estrutura de dados para jogador
typedef struct {
	int32_t id;	// Id do jogador
	char* name; // Nome do jogador
	char* positions; // String com todas as posi��es do jogador
	float averageRating; // M�dia das avalia��es do jogador
	uint32_t ratingCount; // N�mero de avali��es recebidas
} Player;

// Estrutura de dados para avalia��o
typedef struct {
	int32_t playerId; // Id do jogador avaliado
	float score; // Nota dada ao jogador avaliado
} Rating;

// Estrutura de dados para usu�rio
typedef struct {
	int32_t id; // Id do usuario

	// Avalia��es de usu�rio ser�o armazenadas em tabela hash
	Rating* ratingArr; // Array com as avali��es feitas pelo usu�rio
	uint32_t ratingsGiven; // N�mero de avali��es feitas pelo usu�rio
	uint32_t ratingArrSize; // Tamanho do array de avalia��es
	bool isSorted; // "true" se o array de avalia��es est� ordenado e "false", caso contr�rio
} User;

// Tabela hash de jogadores
typedef struct {
	Player* arr; // Array com dados de jogadores
	uint32_t M; // Tamanho da tabela
	uint32_t N; // N�mero de dados a serem inseridos
} PlayerHash;

// Nodo para lista e jogadores
typedef struct playerListNode {
	Player* player; // Ponteiro para jogador
	struct playerListNode* next; // Pr�ximo nodo da lista
} PlayerListNode;

// Lista de jogadores
typedef struct {
	PlayerListNode* fst; // Ponteiro para primeiro nodo da lista
} PlayerList;

/*
 * Nodo para lista de posi��es(estrutura que armazenar� uma posi��o e a lista de jogadores
 * que jogam nesta posi��o
 */
typedef struct positionListNode {
	char* acronym; // Acr�nimo (abreviatura) da posi��o
	PlayerList playerList; // Lista de jogadores que jogam nessa posi��o
	struct positionListNode* next; // Pr�ximo nodo da lista
} PositionListNode;

// Lista de posi��es
typedef struct {
	PositionListNode* fst; // Ponteiro para primeiro nodo da lista
} PositionList;

// Tabela hash de usu�rios
typedef struct {
	User* arr; // Array com dados de usu�rios
	uint32_t M; // Tamanho da tabela
	uint32_t N; // N�mero de dados a serem inseridos
} UserHash;

// Estrutura que armazenar� uma tag e dados relacionados a essa tag
typedef struct {
	char* data; // Tag

	// O id dos jogadores que receberam a tag ser�o armazenadas em uma tabela hash
	int32_t* idArr; // Array com ids de jogadores que receberam a tag
	uint32_t idArrSize; // Tamanho do array
	uint32_t idCount; // N�meros de jogadores que receberam a tag
} Tag;

// Tabela hash de tags
typedef struct {
	Tag* arr; // Array com dados de tags
	uint32_t M; // Tamanho da tabela
	uint32_t N; // N�mero de dados a serem inseridos
} TagHash;

// Lista de Ids de jogadores
typedef struct playerIdList {
	int32_t id; // Id de um jogador
	struct playerIdList* next; // Pr�ximo nodo da lista
} PlayerIdList;

// Trie para pesquisa por prefixo sobre nomes de jogadores
typedef struct trieNode {
	/*
	 * Estrutura auxiliar para armazenar lista de nodos filhos (substituindo array com alfabeto completo).
	 * No nodo ra�z da �rvore, por exemplo, essa lista conter� todos os caracteres que podem ser o primeiro
	 * caractere do nome de algum jogador. Esses caracteres s�o salvos no campo "associatedChar".
	 */
	struct nodeList {
		char associatedChar; // Caractere associado ao nodo filho
		struct trieNode* ptNextLevel; // Ponteiro para o pr�ximo n�vel da �rvore
		struct nodeList* next; // Ponteiro para o pr�ximo elemento da lista
	} *childrenList;

	// Lista de ids de jogadores associados ao nodo (jogadores cujo nome, quando inserido na �rvore, termina neste nodo).
	PlayerIdList* idList;
} Trie;

struct nodeList TrieNodeList;

// Procedimentos para implementa��o de tabelas hash com dados dos jogadores.
Player* queryPlayer(PlayerHash playerHash, int32_t key); // Busca jogador na tabela hash
void savePlayerData(FILE* fp, PlayerHash* playerHash); // Salva dados de jogadores na tabela hash

// Procedimentos para armazenar avalia��es de jogadores e informa��es de usu�rios.
void saveRatings(FILE* fp, PlayerHash* playerHash, UserHash* userHash);

// Procedimentos para armazenar tags de jogadores.
void saveTags(FILE* fp, TagHash* tagHash);

// Agrupa jogadores em listas definidas de acordo com suas posi��es.
void groupPlayersByPosition(PositionList* positionList, PlayerHash playerHash);
void showTopPlayers(PositionList positionList, char* position, uint32_t N);

// Implementa��o da �rvore trie para busca sobre prefixos de nomes de jogadores.
void makePlayerNameTrie(Trie* root, PlayerHash playerHash); // Cria nova �rvore trie com nomes dos jogadores
void wildcardQuery(Trie* root, PlayerHash playerHash, char* key); // Busca por nomes de jogadores na trie pelo prefixo "key"

// Procedimentos para a realiza��o de pesquisas sobre jogadores revisados por usu�rios.
void showUserRatings(int32_t userId, UserHash* userHash, PlayerHash playerHash);

// Procedimentos para a realiza��o de pesquisas por tags de jogadores.
void tagQuery(char** tags, int32_t numberOfTags, TagHash* tagHash, PlayerHash playerHash);

#endif // FIFADB_H
