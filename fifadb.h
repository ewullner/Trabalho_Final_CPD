#ifndef FIFADB_H
#define FIFADB_H

#define STR_BUFFER_SIZE 200 // Buffer para strings (usado principalmente para armazenar linhas dos arquivos)
#define NUMBER_OF_PLAYERS 18944 // Número de jogadores a serem armazenados

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "conio2.h"
#include <string.h>

// Estrutura de dados para jogador
typedef struct {
	int32_t id;	// Id do jogador
	char* name; // Nome do jogador
	char* positions; // String com todas as posições do jogador
	float averageRating; // Média das avaliações do jogador
	uint32_t ratingCount; // Número de avalições recebidas
} Player;

// Estrutura de dados para avaliação
typedef struct {
	int32_t playerId; // Id do jogador avaliado
	float score; // Nota dada ao jogador avaliado
} Rating;

// Estrutura de dados para usuário
typedef struct {
	int32_t id; // Id do usuario

	// Avaliações de usuário serão armazenadas em tabela hash
	Rating* ratingArr; // Array com as avalições feitas pelo usuário
	uint32_t ratingsGiven; // Número de avalições feitas pelo usuário
	uint32_t ratingArrSize; // Tamanho do array de avaliações
	bool isSorted; // "true" se o array de avaliações está ordenado e "false", caso contrário
} User;

// Tabela hash de jogadores
typedef struct {
	Player* arr; // Array com dados de jogadores
	uint32_t M; // Tamanho da tabela
	uint32_t N; // Número de dados a serem inseridos
} PlayerHash;

// Nodo para lista e jogadores
typedef struct playerListNode {
	Player* player; // Ponteiro para jogador
	struct playerListNode* next; // Próximo nodo da lista
} PlayerListNode;

// Lista de jogadores
typedef struct {
	PlayerListNode* fst; // Ponteiro para primeiro nodo da lista
} PlayerList;

/*
 * Nodo para lista de posições(estrutura que armazenará uma posição e a lista de jogadores
 * que jogam nesta posição
 */
typedef struct positionListNode {
	char* acronym; // Acrônimo (abreviatura) da posição
	PlayerList playerList; // Lista de jogadores que jogam nessa posição
	struct positionListNode* next; // Próximo nodo da lista
} PositionListNode;

// Lista de posições
typedef struct {
	PositionListNode* fst; // Ponteiro para primeiro nodo da lista
} PositionList;

// Tabela hash de usuários
typedef struct {
	User* arr; // Array com dados de usuários
	uint32_t M; // Tamanho da tabela
	uint32_t N; // Número de dados a serem inseridos
} UserHash;

// Estrutura que armazenará uma tag e dados relacionados a essa tag
typedef struct {
	char* data; // Tag

	// O id dos jogadores que receberam a tag serão armazenadas em uma tabela hash
	int32_t* idArr; // Array com ids de jogadores que receberam a tag
	uint32_t idArrSize; // Tamanho do array
	uint32_t idCount; // Números de jogadores que receberam a tag
} Tag;

// Tabela hash de tags
typedef struct {
	Tag* arr; // Array com dados de tags
	uint32_t M; // Tamanho da tabela
	uint32_t N; // Número de dados a serem inseridos
} TagHash;

// Lista de Ids de jogadores
typedef struct playerIdList {
	int32_t id; // Id de um jogador
	struct playerIdList* next; // Próximo nodo da lista
} PlayerIdList;

// Trie para pesquisa por prefixo sobre nomes de jogadores
typedef struct trieNode {
	/*
	 * Estrutura auxiliar para armazenar lista de nodos filhos (substituindo array com alfabeto completo).
	 * No nodo raíz da árvore, por exemplo, essa lista conterá todos os caracteres que podem ser o primeiro
	 * caractere do nome de algum jogador. Esses caracteres são salvos no campo "associatedChar".
	 */
	struct nodeList {
		char associatedChar; // Caractere associado ao nodo filho
		struct trieNode* ptNextLevel; // Ponteiro para o próximo nível da árvore
		struct nodeList* next; // Ponteiro para o próximo elemento da lista
	} *childrenList;

	// Lista de ids de jogadores associados ao nodo (jogadores cujo nome, quando inserido na árvore, termina neste nodo).
	PlayerIdList* idList;
} Trie;

struct nodeList TrieNodeList;

// Procedimentos para implementação de tabelas hash com dados dos jogadores.
Player* queryPlayer(PlayerHash playerHash, int32_t key); // Busca jogador na tabela hash
void savePlayerData(FILE* fp, PlayerHash* playerHash); // Salva dados de jogadores na tabela hash

// Procedimentos para armazenar avaliações de jogadores e informações de usuários.
void saveRatings(FILE* fp, PlayerHash* playerHash, UserHash* userHash);

// Procedimentos para armazenar tags de jogadores.
void saveTags(FILE* fp, TagHash* tagHash);

// Agrupa jogadores em listas definidas de acordo com suas posições.
void groupPlayersByPosition(PositionList* positionList, PlayerHash playerHash);
void showTopPlayers(PositionList positionList, char* position, uint32_t N);

// Implementação da árvore trie para busca sobre prefixos de nomes de jogadores.
void makePlayerNameTrie(Trie* root, PlayerHash playerHash); // Cria nova árvore trie com nomes dos jogadores
void wildcardQuery(Trie* root, PlayerHash playerHash, char* key); // Busca por nomes de jogadores na trie pelo prefixo "key"

// Procedimentos para a realização de pesquisas sobre jogadores revisados por usuários.
void showUserRatings(int32_t userId, UserHash* userHash, PlayerHash playerHash);

// Procedimentos para a realização de pesquisas por tags de jogadores.
void tagQuery(char** tags, int32_t numberOfTags, TagHash* tagHash, PlayerHash playerHash);

#endif // FIFADB_H
