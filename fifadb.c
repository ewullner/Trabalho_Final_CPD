#include "fifadb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "conio2.h"

#define COUNTING_RANGE 51

// Testa se n é primo.
bool isPrime(uint64_t n) {
	if (n <= 1) return false;
	if (n == 2 || n == 3) return true;
	if (n % 2 == 0 || n % 3 == 0) return false;

	for (int i = 5; i * i <= n; i = i + 6)
		if (n % i == 0 || n % (i + 2) == 0)
			return false;

	return true;
}

// Função básica de hash. "key" é a chave (inteiro) a ser mapeada; "M" é o tamanho da tabela.
size_t simpleHash(int32_t key, size_t M) {
	return (size_t)(key % M);
}

/*
 * Função de hash com método polinomial. "key" é a chave (string) a ser mapeada; "M" é o tamanho da tabela;
 * "len" é o tamanho da string "key".
 */
size_t polynomialRollingHash(char* key, size_t M, size_t len) {
	static const uint32_t p = 31;

	size_t p_pow = p, h = 0;

	for (uint32_t i = 0; i < len; i++) {
		h = (h + (key[i] + 1) * p_pow) % M;
		p_pow = (p_pow * p) % M;
	}
	return h;
}

/*
 * Dado uma entrada "N" representando o número de dados a serem inseridos na tabela,
 * retorna o tamanho ideal a ser alocado para a tabela.
 */
size_t getHashTableSize(size_t N) {
	size_t M = (size_t)ceil((double)(4 * N) / 3);

	// Tamanho da tabela será o menor número primo maior ou igual a 4 * N / 3.
	while (!isPrime(M)) M++;
	return M;
}

/*
 * Cria estrutura para jogador através de informações lidas nas linhas do arquivo "players.csv".
 * Essas informações são lidas e devidamente atribuídas aos parâmetros "id", "name" e "positions"
 * através da função "readPlayersFileRow".
 */
void makePlayer(Player* player, int32_t id, char* name, char* positions, size_t nameLen, size_t positionsLen) {
	player->id = id;

	if ((player->name = (char*)malloc(nameLen + 1)) == NULL) {
		puts("Erro ao alocar memoria.");
		return;
	}
	strcpy(player->name, name);
	player->name[nameLen] = '\0';

	if ((player->positions = (char*)malloc(positionsLen + 1)) == NULL) {
		puts("Erro ao alocar memoria.");
		return;
	}
	strcpy(player->positions, positions);
	player->positions[positionsLen] = '\0';

	player->ratingCount = 0;
	player->averageRating = 0;
}

/*
 * Cria estrutura para avaliação através de informações lidas nas linhas do arquivo "rating.csv".
 * Essas informações são lidas e devidamente atribuídas aos parâmetros "id", "name" e "positions"
 * através da função "readRatingFileRow".
 */
void makeRating(Rating* rating, int32_t playerId, float score) {
	rating->playerId = playerId;
	rating->score = score;
}

/*
 * Cada estrutura do tipo "User" armazenará dados de um usuário e terá um array contendo todas as
 * avaliações de jogadores feitas por este usuário. Nesta função, será alocada memória para esse array
 * (o tamanho do array será definido pelo parâmetro "initialSize", o qual é dado pela função "makeUser")
 * e, para cada avaliação nesse array, será atribuído o valor -1 ao campo "playerId".
 */
void initUserRatingArr(User* user, size_t initialSize) {
	user->ratingArrSize = initialSize;
	user->ratingsGiven = 0;
	user->ratingArr = NULL;
	user->ratingArr = (Rating*)malloc(initialSize * sizeof(Rating));

	if (!user->ratingArr) {
		puts("Erro ao alocar memoria.");
		return;
	}
	for (uint32_t i = 0; i < initialSize; i++)
		user->ratingArr[i].playerId = -1;
}


// Inicializa usuário.
void makeUser(User* user, int32_t id) {
	static const size_t initialRatingArrSize = 23;

	user->id = id;
	initUserRatingArr(user, initialRatingArrSize);
}

// Inicializa tabela hash de jogadores.
void initPlayerHash(PlayerHash* playerHash) {
	// Número de dados a serem inseridos já é conhecido, então foi definido como uma constante.
	playerHash->N = NUMBER_OF_PLAYERS;

	// Atribui ao campo M da tabela o valor retornado por getHashTableSize com N = NUMBER_OF_PLAYERS.
	playerHash->M = getHashTableSize(playerHash->N);

	// Aloca memória para a tabela de jogadores e testa se a memória foi alocada com sucesso.
	playerHash->arr = NULL;
	if ((playerHash->arr = (Player*)malloc(playerHash->M * sizeof(Player))) == NULL) {
		puts("Erro ao alocar memoria.");
		return;
	}
	// Inicializa id de cada jogador da tabela com -1 (não presente entre os ids válidos).
	for (size_t i = 0; i < playerHash->M; i++)
		playerHash->arr[i].id = -1;
}

// Insere novo jogador na tabela e retorna endereço do jogador inserido.
Player* insertPlayerInHash(PlayerHash* playerHash, Player player) {
	size_t h = simpleHash(player.id, playerHash->M);

	// Se o id na posição h for - 1, a posição está livre e o novo jogador pode ser inserido aqui.
	if (playerHash->arr[h].id == -1) {
		playerHash->arr[h] = player;
		return playerHash->arr + h;
	}
	// Se posição h estiver ocupada, utiliza-se Duplo Hashing para tratamento de colisões.

	// p será o maior primo menor que o tamanho da tabela.
	size_t p = playerHash->M - 2;
	while (!isPrime(p)) p -= 2;

	size_t h2 = p - simpleHash(player.id, p);
	size_t probe, i = 1;

	do {
		probe = (h + i * h2) % playerHash->M;
		if (playerHash->arr[probe].id == -1) {
			playerHash->arr[probe] = player;
			return playerHash->arr + probe;
		}
		i++;
	} while (i <= playerHash->M);

	// Este caso não deve ocorrer (será usado para testes caso algum erro ocorra).
	puts("Erro ao armazenar dados de jogador (tabela cheia).");
	return NULL;
}

void initPositionListNode(PositionListNode* node, char* acronym) {
	size_t len = strlen(acronym);

	node->acronym = NULL;
	if ((node->acronym = (char*)malloc(len + 1)) == NULL) {
		puts("Erro ao alocar memoria.");
		return;
	}
	strcpy(node->acronym, acronym);
	node->acronym[len] = '\0';

	node->next = NULL;
	node->playerList.fst = NULL;
}

void initPositionList(PositionList* positionList) {
	positionList->fst = NULL;
}

void insertPlayerInList(PlayerList* playerList, Player* player) {
	if (!playerList->fst) {
		playerList->fst = (PlayerListNode*)malloc(sizeof(PlayerListNode));

		if (!playerList->fst) {
			puts("Erro ao alocar memoria.");
			return;
		}
		playerList->fst->player = player;
		playerList->fst->next = NULL;
		return;
	}
	if (playerList->fst->player->averageRating < player->averageRating) {
		PlayerListNode* newNode = (PlayerListNode*)malloc(sizeof(PlayerListNode));

		if (!newNode) {
			puts("Erro ao alocar memoria.");
			return;
		}
		newNode->next = playerList->fst;
		newNode->player = player;
		playerList->fst = newNode;
		return;
	}
	PlayerListNode* aux = playerList->fst, * prev;

	do {
		prev = aux;
		aux = aux->next;
	} while (aux && aux->player->averageRating >= player->averageRating);

	PlayerListNode* newNode = (PlayerListNode*)malloc(sizeof(PlayerListNode));

	if (!newNode) {
		puts("Erro ao alocar memoria.");
		return;
	}
	newNode->next = prev->next;
	newNode->player = player;
	prev->next = newNode;
}

void insertPosition(PositionList* positionList, Player* player, char* acronym) {
	if (!positionList->fst) {
		if ((positionList->fst = (PositionListNode*)malloc(sizeof(PositionListNode))) == NULL) {
			puts("Erro ao alocar memoria.");
			return;
		}
		initPositionListNode(positionList->fst, acronym);
		insertPlayerInList(&(positionList->fst->playerList), player);
		return;
	}
	PositionListNode* aux = positionList->fst;
	int dif;

	while ((dif = strcmp(aux->acronym, acronym)) != 0 && aux->next)
		aux = aux->next;

	if (dif == 0)
		insertPlayerInList(&(aux->playerList), player);
	else {
		if ((aux->next = (PositionListNode*)malloc(sizeof(PositionListNode))) == NULL) {
			puts("Erro ao alocar memoria.");
			return;
		}
		initPositionListNode(aux->next, acronym);
		insertPlayerInList(&(aux->next->playerList), player);
	}
}

void insertPlayerPositions(PositionList* positionList, Player* player) {
	char* tok = NULL;
	tok = strtok(player->positions, ", ");

	do {
		insertPosition(positionList, player, tok);
	} while ((tok = strtok(NULL, ", ")) != NULL);
}

void groupPlayersByPosition(PositionList* positionList, PlayerHash playerHash) {
	initPositionList(positionList);

	for (uint32_t i = 0; i < playerHash.M; i++)
		if (playerHash.arr[i].id >= 0)
			insertPlayerPositions(positionList, &(playerHash.arr[i]));
}


// Lê linha do arquivo "players.csv", cria novo jogador e o insere na tabela hash.
void readPlayersFileRow(PlayerHash* playerHash, char* row) {
	Player newPlayer;
	char* name = NULL, * positions = NULL;
	size_t nameLen, positionsLen;
	int32_t id;

	id = atoi(strtok(row, ","));

	name = strtok(NULL, ",");
	nameLen = strlen(name);

	/*
	 * No campo "positions" do jogador será armazenada a string contendo as posições deste jogador no
	 * mesmo formato do arquivo, porém sem as aspas.
	 */
	positions = strtok(NULL, "\"\n");
	if (positions[0] == '"') positions++;
	positionsLen = strlen(positions);

	// Cria novo jogador com os dados lidos.
	makePlayer(&newPlayer, id, name, positions, nameLen, positionsLen);

	// Insere novo jogador na tabela hash.
	insertPlayerInHash(playerHash, newPlayer);
}

void showTopPlayers(PositionList positionList, char* position, uint32_t N) {
	PositionListNode* positAux = positionList.fst;

	while (positAux && strcmp(positAux->acronym, position))
		positAux = positAux->next;

	if (!positAux) {
		puts("Posicao nao encontrada.");
		return;
	}
	PlayerListNode* playerAux = positAux->playerList.fst;
	uint32_t i = 0;

	while (playerAux && i < N) {
		if (playerAux->player->ratingCount >= 1000) {
			Player player = *(playerAux->player);
			printf("%d %s %s %.2f %d\n", player.id, player.name, player.positions, player.averageRating, player.ratingCount);
			i++;
		}
		playerAux = playerAux->next;
	}
}


// Lê arquivo "players.csv" e cria tabela hash de jogadores.
void savePlayerData(FILE* fp, PlayerHash* playerHash) {
	initPlayerHash(playerHash); // Inicializa tabela hash.

	// Testa se array da tabela foi alocado.
	if (!playerHash->arr)
		return;

	// Buffer para armazenar linha lida do arquivo.
	char buf[STR_BUFFER_SIZE];
	fgets(buf, STR_BUFFER_SIZE, fp); // Lê cabeçalho.

	// Lê todas as linhas do arquivo, salvando dados de jogadores.
	while (fgets(buf, STR_BUFFER_SIZE, fp))
		readPlayersFileRow(playerHash, buf);


	fclose(fp); // Fecha o arquivo.
}

/*
 *	Inicializa tabela hash de usuários. O tamanho inicial da tabela é dado pelo parâmetro initialSize,
 *	definido na função "saveRatings". Inicialmente a tabela de usuários terá o mesmo tamanho que a tabela
 *	de jogadores; entretanto, pode ser necessário que este tamanho seja alterado.
 */
void initUserHash(UserHash* userHash, size_t initialSize) {
	userHash->arr = NULL;
	if ((userHash->arr = (User*)malloc(initialSize * sizeof(User))) == NULL) {
		puts("Erro ao alocar memoria.");
		return;
	}
	userHash->M = initialSize;
	userHash->N = 0;

	/* Após alocar memória para uma quantidade inicial de usuários, atribui ao
	campo id o de cada usuário o valor -1 (inválido). */
	for (size_t i = 0; userHash->arr + i && i < initialSize; i++)
		userHash->arr[i].id = -1;
}

/*
 *	Se o usuário não tiver sido inserido na tabela, insere-o e retorna seu endereço. A função de hashing
 *  e a técnica para tratamento de colisões são idênticas às utilizadas para a tabela de jogadores.
 */
User* insertUserInHash(UserHash* userHash, User user) {
	size_t h = simpleHash(user.id, userHash->M);

	if (userHash->arr[h].id == -1) {
		userHash->arr[h] = user;
		return userHash->arr + h;
	}
	if (userHash->arr[h].id == user.id)
		return NULL;

	size_t p = userHash->M - 2;
	while (!isPrime(p)) p -= 2;

	size_t h2 = p - simpleHash(user.id, p);
	size_t probe, i = 1;

	do {
		probe = (h + i * h2) % userHash->M;
		if (userHash->arr[probe].id < 0) {
			userHash->arr[probe] = user;
			return userHash->arr + h;
		}
		if (userHash->arr[probe].id == user.id)
			return NULL;
		i++;
	} while (i <= userHash->M);

	puts("Erro ao armazenar dados de usuario (tabela cheia).");
	return NULL;
}

// Se o tamanho da tabela deve ser incrementado, retorna true; senão, retorna false.
bool isHashIncreaseNeeded(size_t M, size_t N) {
	if (M <= N)
		return true;
	return false;
}

// Se o tamanho da tabela deve ser decrementado, retorna true; senão, retorna false.
bool isHashDecreaseNeeded(size_t M, size_t N) {
	if (M > 2 * N)
		return true;
	return false;
}

/*
 * Incrementa ou decrementa o tamanho ta tabela hash de usuários. Essa função sempre redimensiona
 * a tabela de modo que o tamanho desta seja o menor número primo maior ou igual a 4 * N / 3.
 * Essa função não é responsável por testar se o redimensionamento da tabela é de fato necessário.
 */
void resizeUserHash(UserHash* userHash) {
	User* tempArr = userHash->arr;
	size_t oldM = userHash->M;

	userHash->M = getHashTableSize(userHash->N);

	userHash->arr = NULL;
	if ((userHash->arr = (User*)malloc(userHash->M * sizeof(User))) == NULL) {
		puts("Erro ao redimensionar tabela de usuarios.");
		return;
	}
	for (size_t i = 0; userHash->arr + i && i < userHash->M; i++)
		userHash->arr[i].id = -1;

	for (size_t i = 0; i < oldM; i++)
		if (tempArr[i].id > 0)
			insertUserInHash(userHash, tempArr[i]);

	free(tempArr);
}

/*
 * Procura jogador na tabela e retorna seu endereço de memória.
 * A busca é feita através do id do jogador, dado pelo parâmetro "key".
 */
Player* queryPlayer(PlayerHash playerHash, int32_t key) {
	size_t h = simpleHash(key, playerHash.M);

	if (playerHash.arr[h].id == key)
		return playerHash.arr + h;

	if (playerHash.arr[h].id == -1)
		return NULL;

	size_t p = playerHash.M - 2;
	while (!isPrime(p)) p -= 2;

	size_t h2 = p - simpleHash(key, p);
	size_t i = 1, probe;

	do {
		probe = (h + i * h2) % playerHash.M;
		if (playerHash.arr[probe].id == key)
			return playerHash.arr + probe;

		if (playerHash.arr[probe].id == -1)
			return NULL;
		i++;
	} while (i <= playerHash.M);

	return NULL;
}

/*
 * Procura usuário na tabela. Se este for encontrado, atribui seu índice na tabela ao parâmetro
 * "index" e retorna "true"; senão, atribui o índice da primeira posição vazia encontrada na busca
 * ao parâmetro "index" e retorna "false". Esses procedimentos serão utilizados ao ler uma linha do
 * arquivo "rating.csv" para verificar se os dados da linha lida já forma armazenados na tabela hash
 * (diferenciando, assim, os usuários que precisam ser criados dos que precisam apenas ser atualizados).
 */
bool queryUser(UserHash userHash, int32_t key, int64_t* index) {
	size_t h = simpleHash(key, userHash.M);

	if (userHash.arr[h].id == -1) {
		*index = h;
		return false;
	}
	if (userHash.arr[h].id == key) {
		*index = h;
		return true;
	}
	size_t p = userHash.M - 2;
	while (!isPrime(p)) p -= 2;

	size_t h2 = p - simpleHash(key, p);
	size_t i = 1, probe;

	do {
		probe = (h + i * h2) % userHash.M;
		if (userHash.arr[probe].id == -1) {
			*index = probe;
			return false;
		}
		if (userHash.arr[probe].id == key) {
			*index = probe;
			return true;
		}
		i++;
	} while (i <= userHash.M);

	*index = -1;
	return false;
}

/*
 * Insere nova avaliação na tabela de avaliações do usuário apontado. A função de hashing e
 * o tratamento de colisões são idênticas às utilizadas para a tabela de jogadores e de usuários.
 */
Rating* insertRatingInHash(User* user, Rating newRating) {
	size_t h = simpleHash(newRating.playerId, user->ratingArrSize);

	if (user->ratingArr[h].playerId == -1) {
		user->ratingArr[h] = newRating;
		return user->ratingArr + h;
	}
	size_t p = user->ratingArrSize - 2;
	while (!isPrime(p)) p -= 2;

	size_t h2 = p - simpleHash(newRating.playerId, p);
	size_t i = 1, probe;

	do {
		probe = (h + i * h2) % user->ratingArrSize;

		if (user->ratingArr[probe].playerId < 0) {
			user->ratingArr[h] = newRating;
			return user->ratingArr + probe;
		}
		i++;
	} while (i <= user->ratingArrSize);

	puts("Erro ao armazenar avaliacao (tabela cheia).");
	return NULL;
}

/*
 * Redimensiona tabela hash de avaliações de usuário. Esse redimensionamento é realizado
 * de forma idêntica ao utilizado para a tabela hash de usuários.
 */
void resizeRatingHash(User* user) {
	Rating* tempArr = user->ratingArr;
	size_t oldM = user->ratingArrSize;

	user->ratingArrSize = getHashTableSize(user->ratingsGiven);

	user->ratingArr = NULL;
	if ((user->ratingArr = (Rating*)malloc(user->ratingArrSize * sizeof(Rating))) == NULL) {
		puts("Erro ao redimensionar tabela de avaliacoes.");
		return;
	}
	for (size_t i = 0; user->ratingArr + i && i < user->ratingArrSize; i++)
		user->ratingArr[i].playerId = -1;

	for (size_t i = 0; i < oldM; i++)
		if (tempArr[i].playerId >= 0)
			insertRatingInHash(user, tempArr[i]);

	free(tempArr);
}

/*
 * Lê linha do arquivo "rating.csv". Atualiza média e contagem de avaliações de jogador,
 * insere novo usuário na tabela se este não existir e insere nova avaliação na tabela hash
 * de avaliações do usuário.
 */
void readRatingFileRow(PlayerHash* playerHash, UserHash* userHash, char* row) {
	int32_t userId, playerId;
	Player* player;

	userId = atoi(strtok(row, ","));
	playerId = atoi(strtok(NULL, ","));

	if ((player = queryPlayer(*playerHash, playerId)) != NULL) {
		int64_t index;
		float score = atof(strtok(NULL, "\n"));
		Rating rating;
		makeRating(&rating, playerId, score);

		// Atualiza informações de usuário
		if (queryUser(*userHash, userId, &index)) {
			insertRatingInHash(&(userHash->arr[index]), rating);
			userHash->arr[index].ratingsGiven++;

			if (isHashIncreaseNeeded(userHash->arr[index].ratingArrSize,
				userHash->arr[index].ratingsGiven))
				resizeRatingHash(&(userHash->arr[index]));
		}
		// Cria novo usuário e o insere na tabela
		else {
			User newUser;
			makeUser(&newUser, userId);
			insertRatingInHash(&newUser, rating);
			newUser.ratingsGiven = 1;
			newUser.isSorted = false;
			userHash->arr[index] = newUser;
			userHash->N++;

			if (isHashIncreaseNeeded(userHash->M, userHash->N))
				resizeUserHash(userHash);
		}
		// Atualiza score de jogador
		player->averageRating = (player->averageRating * player->ratingCount + score) / (player->ratingCount + 1);
		player->ratingCount++;
	}
}

/*
 * Lê arquivo "rating.csv". Define média de avaliações e número de avaliações de jogadores,
 * cria tabela hash de usuários e, para cada usuário da tabela, cria uma tabela hash com
 * as avaliações feitas pelo usuário.
 */
void saveRatings(FILE* fp, PlayerHash* playerHash, UserHash* userHash) {
	initUserHash(userHash, playerHash->M);

	if (!userHash->arr)
		return;

	char buf[STR_BUFFER_SIZE];
	fgets(buf, STR_BUFFER_SIZE, fp); // Lê cabeçalho

	while (fgets(buf, STR_BUFFER_SIZE, fp))
		readRatingFileRow(playerHash, userHash, buf);

	if (isHashDecreaseNeeded(userHash->M, userHash->N))
		resizeUserHash(userHash);

	for (uint32_t i = 0; i < userHash->M; i++)
		if (userHash->arr[i].id >= 0)
			if (isHashDecreaseNeeded(userHash->arr[i].ratingArrSize, userHash->arr[i].ratingsGiven))
				resizeRatingHash(&(userHash->arr[i]));

	fclose(fp);
}

void initTagHash(TagHash* tagHash, uint32_t initialSize) {
	tagHash->arr = NULL;
	if ((tagHash->arr = (Tag*)malloc(initialSize * sizeof(Tag))) == NULL) {
		puts("Erro ao alocar memoria.");
		return;
	}
	tagHash->M = initialSize;
	tagHash->N = 0;

	for (uint32_t i = 0; i < initialSize; i++) {
		tagHash->arr[i].data = NULL;
		tagHash->arr[i].idArr = NULL;
	}
}

void makeNewTag(Tag* tag, char* data, size_t len) {
	static const uint32_t initialIdArrSize = 11;

	if ((tag->data = (char*)malloc(len + 1)) == NULL) {
		puts("Erro ao alocar memoria.");
		return;
	}
	strcpy(tag->data, data);
	tag->data[len] = '\0';

	tag->idArr = (int32_t*)malloc(initialIdArrSize * sizeof(int32_t));

	if (tag->idArr == NULL) {
		puts("Erro ao alocar memoria.");
		return;
	}
	tag->idArrSize = initialIdArrSize;
	tag->idCount = 0;

	for (uint32_t i = 0; i < initialIdArrSize; i++)
		tag->idArr[i] = -1;
}

Tag* insertTagInHash(TagHash* tagHash, char* data) {
	size_t len = strlen(data);
	uint32_t h = polynomialRollingHash(data, tagHash->M, len);

	if (tagHash->arr[h].data == NULL) {
		makeNewTag(&(tagHash->arr[h]), data, len);
		tagHash->N++;
		return tagHash->arr + h;
	}
	if ((strcmp(tagHash->arr[h].data, data)) == 0)
		return tagHash->arr + h;

	uint32_t p = tagHash->M - 2;
	while (!isPrime(p)) p -= 2;

	uint32_t h2 = p - polynomialRollingHash(data, p, len);
	uint32_t probe, i = 1;

	do {
		probe = (h + i * h2) % tagHash->M;
		if (tagHash->arr[probe].data == NULL) {
			makeNewTag(&(tagHash->arr[probe]), data, len);
			tagHash->N++;
			return tagHash->arr + probe;
		}
		if ((strcmp(tagHash->arr[probe].data, data)) == 0)
			return tagHash->arr + probe;
		i++;
	} while (i <= tagHash->M);

	puts("Erro ao armazenar tag (tabela cheia).");
	return NULL;
}

void insertTagInResizedHash(TagHash* tagHash, Tag tag) {
	size_t len = strlen(tag.data);
	uint32_t h = polynomialRollingHash(tag.data, tagHash->M, len);

	if (tagHash->arr[h].data == NULL) {
		if ((tagHash->arr[h].data = (char*)malloc(strlen(tag.data) + 1)) == NULL) {
			puts("Erro ao alocar memoria.");
			return NULL;
		}
		strcpy(tagHash->arr[h].data, tag.data);

		if ((tagHash->arr[h].idArr = (int32_t*)malloc(tag.idArrSize * sizeof(int32_t))) == NULL) {
			puts("Erro ao alocar memoria.");
			return NULL;
		}
		memcpy(tagHash->arr[h].idArr, tag.idArr, tag.idArrSize * sizeof(int32_t));

		tagHash->arr[h].idArrSize = tag.idArrSize;
		tagHash->arr[h].idCount = tag.idCount;
		return;
	}
	uint32_t p = tagHash->M - 2;
	while (!isPrime(p)) p -= 2;

	uint32_t h2 = p - polynomialRollingHash(tag.data, p, len);
	uint32_t probe, i = 1;

	do {
		probe = (h + i * h2) % tagHash->M;
		if (tagHash->arr[probe].data == NULL) {
			tagHash->arr[probe].data = (char*)malloc(strlen(tag.data) + 1);
			strcpy(tagHash->arr[probe].data, tag.data);

			tagHash->arr[probe].idArr = (int32_t*)malloc(tag.idArrSize * sizeof(int32_t));
			memcpy(tagHash->arr[probe].idArr, tag.idArr, tag.idArrSize * sizeof(int32_t));

			tagHash->arr[probe].idArrSize = tag.idArrSize;
			tagHash->arr[probe].idCount = tag.idCount;
			return;
		}
		i++;
	} while (i <= tagHash->M);

	puts("Erro ao armazenar tag (tabela cheia).");
}

void resizeTagHash(TagHash* tagHash) {
	Tag* tempArr = tagHash->arr;
	uint32_t oldM = tagHash->M;

	tagHash->M = getHashTableSize(tagHash->N);

	tagHash->arr = NULL;
	if ((tagHash->arr = (Tag*)malloc(tagHash->M * sizeof(Tag))) == NULL) {
		puts("Erro ao alocar memoria.");
		return;
	}

	for (uint32_t i = 0; i < tagHash->M; i++) {
		tagHash->arr[i].data = NULL;
		tagHash->arr[i].idArr = NULL;
	}
	for (uint32_t i = 0; i < oldM; i++)
		if (tempArr[i].data) {
			insertTagInResizedHash(tagHash, tempArr[i]);
			free(tempArr[i].data);
			free(tempArr[i].idArr);
		}

	free(tempArr);
}

bool insertIdInHash(Tag* tag, int32_t id) {
	uint32_t h = simpleHash(id, tag->idArrSize);

	if (tag->idArr[h] == -1) {
		tag->idArr[h] = id;
		tag->idCount++;
		return true;
	}
	if (tag->idArr[h] == id)
		return false;

	uint32_t p = tag->idArrSize - 1;
	while (!isPrime(p)) p--;

	uint32_t h2 = p - simpleHash(id, p);
	uint32_t probe, i = 1;

	do {
		probe = (h + i * h2) % tag->idArrSize;
		if (tag->idArr[probe] == -1) {
			tag->idArr[probe] = id;
			tag->idCount++;
			return true;
		}
		if (tag->idArr[probe] == id)
			return false;
		i++;
	} while (i <= tag->idArrSize);

	puts("Erro ao armazenar id (tabela cheia).");
	return false;
}

void insertIdInResizedHash(Tag* tag, int32_t id) {
	uint32_t h = simpleHash(id, tag->idArrSize);

	if (tag->idArr[h] == -1) {
		tag->idArr[h] = id;
		return;
	}

	uint32_t p = tag->idArrSize - 1;
	while (!isPrime(p)) p--;

	uint32_t h2 = p - simpleHash(id, p);
	uint32_t probe, i = 1;

	do {
		probe = (h + i * h2) % tag->idArrSize;
		if (tag->idArr[probe] == -1) {
			tag->idArr[probe] = id;
			return;
		}
		i++;
	} while (i <= tag->idArrSize);

	puts("Erro ao armazenar id (tabela cheia).");
}

void resizeIdHash(Tag* tag) {
	int32_t* tempArr = tag->idArr;
	uint32_t oldM = tag->idArrSize;

	tag->idArrSize = getHashTableSize(tag->idCount);

	tag->idArr = NULL;
	if ((tag->idArr = (int32_t*)malloc(tag->idArrSize * sizeof(int32_t))) == NULL) {
		puts("Erro ao alocar memoria.");
		return;
	}

	for (uint32_t i = 0; i < tag->idArrSize; i++)
		tag->idArr[i] = -1;

	for (uint32_t i = 0; i < oldM; i++)
		if (tempArr[i] >= 0)
			insertIdInResizedHash(tag, tempArr[i]);

	free(tempArr);
}

void saveTags(FILE* fp, TagHash* tagHash) {
	uint32_t tagHashInitialSize = getHashTableSize(NUMBER_OF_PLAYERS / 4);
	char buf[STR_BUFFER_SIZE];
	char* tagRead = NULL;
	Tag* tagPtr = NULL;
	int32_t id;

	initTagHash(tagHash, tagHashInitialSize);

	fgets(buf, STR_BUFFER_SIZE, fp); // Lê cabeçalho

	while (fgets(buf, STR_BUFFER_SIZE, fp)) {
		strtok(buf, ",");
		id = atoi(strtok(NULL, ","));
		tagRead = strtok(NULL, "\n");

		tagPtr = insertTagInHash(tagHash, tagRead);

		insertIdInHash(tagPtr, id);

		if (isHashIncreaseNeeded(tagPtr->idArrSize, tagPtr->idCount))
			resizeIdHash(tagPtr);

		if (isHashIncreaseNeeded(tagHash->M, tagHash->N))
			resizeTagHash(tagHash);
	}
	if(isHashDecreaseNeeded(tagHash->M, tagHash->N))
		resizeTagHash(tagHash);

	for (uint32_t i = 0; i < tagHash->M; i++)
		if (tagHash->arr[i].data)
			if (isHashDecreaseNeeded(tagHash->arr[i].idArrSize, tagHash->arr[i].idCount))
				resizeIdHash(&(tagHash->arr[i]));

	fclose(fp);
}

void initTrieNode(Trie* node) {
	node->childrenList = NULL;
	node->idList = NULL;
}

PlayerIdList* insertIdInList(PlayerIdList* idList, int32_t id) {
	PlayerIdList* newNode = (PlayerIdList*)malloc(sizeof(PlayerIdList));

	if (!newNode) {
		puts("Erro ao alocar memoria.");
		return;
	}
	newNode->id = id;
	newNode->next = idList;
	idList = newNode;
	return idList;
}

void insertPlayerNameInTrie(Trie* root, char* playerName, int32_t id) {
	size_t len = strlen(playerName);
	int32_t level = 0;
	char key;
	Trie* pCrawl = root;

	while (level < len) {
		key = playerName[level];

		if (!pCrawl->childrenList) {
			if ((pCrawl->childrenList = (struct nodeList*)malloc(sizeof(struct nodeList))) == NULL) {
				puts("Erro ao alocar memoria.");
				return;
			}
			pCrawl->childrenList->associatedChar = key;
			pCrawl->childrenList->next = NULL;

			if ((pCrawl->childrenList->ptNextLevel = (Trie*)malloc(sizeof(Trie))) == NULL) {
				puts("Erro ao alocar memoria.");
				return;
			}
			initTrieNode(pCrawl->childrenList->ptNextLevel);
			pCrawl = pCrawl->childrenList->ptNextLevel;
		}
		else {
			struct nodeList* auxList = pCrawl->childrenList;

			while (auxList->next && auxList->associatedChar != key)
				auxList = auxList->next;

			if (auxList->associatedChar == key)
				pCrawl = auxList->ptNextLevel;
			else {
				if ((auxList->next = (struct nodeList*)malloc(sizeof(struct nodeList))) == NULL) {
					puts("Erro ao alocar memoria.");
					return;
				}
				auxList->next->associatedChar = key;
				auxList->next->next = NULL;

				if ((auxList->next->ptNextLevel = (Trie*)malloc(sizeof(Trie))) == NULL) {
					puts("Erro ao alocar memoria.");
					return;
				}
				initTrieNode(auxList->next->ptNextLevel);
				pCrawl = auxList->next->ptNextLevel;
			}
		}
		level++;
	}
	pCrawl->idList = insertIdInList(pCrawl->idList, id);
}

void makePlayerNameTrie(Trie* root, PlayerHash playerHash) {
	initTrieNode(root);

	for (uint32_t i = 0; i < playerHash.M; i++)
		if (playerHash.arr[i].id >= 0)
			insertPlayerNameInTrie(root, playerHash.arr[i].name, playerHash.arr[i].id);
}

void collect(Trie* node, PlayerHash playerHash) {
	if (node->idList != NULL) {
		PlayerIdList* aux = node->idList;

		while (aux) {
			Player* player = queryPlayer(playerHash, aux->id);

			if (player)
				printf("%d %s %s %.6f %d\n", player->id, player->name, player->positions, player->averageRating, player->ratingCount);

			aux = aux->next;
		}
	}
	struct nodeList* aux = node->childrenList;

	while (aux) {
		collect(aux->ptNextLevel, playerHash);
		aux = aux->next;
	}
}

void wildcardQuery(Trie* root, PlayerHash playerHash, char* key) {
	size_t len = strlen(key);
	int32_t level = 0;
	char c;
	Trie* pCrawl = root;

	while (level < len) {
		c = key[level];

		struct nodeList* aux = pCrawl->childrenList;

		while (aux && aux->associatedChar != c)
			aux = aux->next;

		if (!aux) {
			puts("Jogador nao encontrado.");
			return;
		}
		pCrawl = aux->ptNextLevel;
		level++;
	}
	collect(pCrawl, playerHash);
    fflush(stdin);
    getchar();
}

User* removeFreeSpaces(User* user) {
	Rating* temp = NULL;
	temp = (Rating*)malloc(user->ratingArrSize * sizeof(Rating));

	if (!temp) {
		puts("Erro ao alocar memoria.");
		return NULL;
	}
	size_t i = 0, j = 0;

	while (i < user->ratingArrSize) {
		if (user->ratingArr[i].playerId >= 0) {
			temp[j] = user->ratingArr[i];
			j++;
		}
		i++;
	}
	free(user->ratingArr);
	user->ratingArr = NULL;
	user->ratingArr = (Rating*)malloc(j * sizeof(Rating));

	if (!user->ratingArr) {
		puts("Erro ao alocar memoria.");
		return NULL;
	}
	user->ratingArrSize = j;

	for (i = 0; i < j; i++)
		user->ratingArr[i] = temp[i];

	free(temp);
	return user;
}

void sortRatings(User* user) {
	Rating* output = NULL;
	output = (Rating*)malloc(user->ratingArrSize * sizeof(Rating));

	if (!output) {
		puts("Erro ao alocar memoria.");
		return;
	}

	if (user->ratingArrSize != user->ratingsGiven)
		user = removeFreeSpaces(user);

	if (!user->ratingArr) {
		puts("Erro ao alocar memoria.");
		return;
	}

	int32_t count[COUNTING_RANGE];
	int32_t i;

	memset(count, 0, sizeof(count));

	for (i = 0; i < user->ratingArrSize; ++i)
		++count[(int32_t)(user->ratingArr[i].score * 10)];

	for(i = 1; i < COUNTING_RANGE; ++i)
		count[i] += count[i - 1];

	for (i = user->ratingArrSize - 1; i >= 0; --i) {
		output[count[(int32_t)(user->ratingArr[i].score * 10)] - 1] = user->ratingArr[i];
		--count[(int32_t)(user->ratingArr[i].score * 10)];
	}

	for (i = 0; i < user->ratingArrSize; ++i)
		user->ratingArr[user->ratingArrSize - i - 1] = output[i];

	free(output);
	user->isSorted = true;
}

void showUserRatings(int32_t userId, UserHash* userHash, PlayerHash playerHash) {
	size_t index;

	if ((queryUser(*userHash, userId, &index)) == false) {
		puts("Usuario nao encontrado.");
		return;
	}

	if (userHash->arr[index].isSorted == false) {
		sortRatings(&(userHash->arr[index]));
		if (userHash->arr[index].isSorted == false) {
			puts("Erro ao mostrar avaliacoes de usuario.");
			return;
		}
	}
	int32_t i = 0;
	Player* player;

	while (i < userHash->arr[index].ratingArrSize && i < 20) {
		player = queryPlayer(playerHash, userHash->arr[index].ratingArr[i].playerId);

		if (player)
			printf("%d %s %.2f %.2f\n", player->id, player->name,
				player->averageRating, userHash->arr[index].ratingArr[i].score);
		i++;
	}
}

Tag* queryTag(TagHash* tagHash, char* key) {
	size_t len = strlen(key);
	uint32_t h = polynomialRollingHash(key, tagHash->M, len);

	if (tagHash->arr[h].data == NULL) {
		puts("Nenhum jogador encontrado com tags pesquisadas.");
		return NULL;
	}
	if ((strcmp(tagHash->arr[h].data, key)) == 0)
		return tagHash->arr + h;

	uint32_t p = tagHash->M - 2;
	while (!isPrime(p)) p -= 2;

	uint32_t h2 = p - polynomialRollingHash(key, p, len);
	uint32_t probe, i = 1;

	do {
		probe = (h + i * h2) % tagHash->M;
		if (tagHash->arr[probe].data == NULL) {
			puts("Nenhum jogador encontrado com tags pesquisadas.");
            return NULL;
		}
		if ((strcmp(tagHash->arr[probe].data, key)) == 0)
			return tagHash->arr + probe;
		i++;
	} while (i <= tagHash->M);

	puts("Nenhum jogador encontrado com tags pesquisadas.");
	return NULL;
}

int32_t queryPlayerId(Tag* tag, int32_t id) {
	uint32_t h = simpleHash(id, tag->idArrSize);

	if (tag->idArr[h] == -1)
		return -1;
	if (tag->idArr[h] == id)
        return id;

	uint32_t p = tag->idArrSize - 1;
	while (!isPrime(p)) p--;

	uint32_t h2 = p - simpleHash(id, p);
	uint32_t probe, i = 1;

	do {
		probe = (h + i * h2) % tag->idArrSize;
		if (tag->idArr[probe] == -1)
			return -1;
        if (tag->idArr[probe] == id)
            return id;
		i++;
	} while (i <= tag->idArrSize);
	return -1;
}

void tagQuery(char** tags, int32_t numberOfTags, TagHash* tagHash, PlayerHash playerHash) {
    Tag* firstTag = queryTag(tagHash, tags[0]);

    if(!firstTag)
        return;

    PlayerIdList* idList = NULL;
    int32_t id, j;
    Player* player;

    for(int32_t i = 0; i < firstTag->idArrSize; i++) {
        id = firstTag->idArr[i];
        j = numberOfTags - 1;
        while(id != -1 && j > 0) {
            Tag* nextTag = queryTag(tagHash, tags[j]);

            if(!nextTag)
                return;

            id = queryPlayerId(nextTag, id);
            j--;
        }
        if (id != -1) {
            PlayerIdList* newNode = (PlayerIdList*)malloc(sizeof(PlayerIdList));

			if (!newNode) {
				puts("Erro ao alocar memoria.");
				return;
			}
            newNode->id = id;
            newNode->next = idList;
            idList = newNode;
        }
    }
	if(!idList)
		puts("Nenhum jogador encontrado com tags pesquisadas.");
	else {
		while (idList) {
			player = queryPlayer(playerHash, idList->id);

			if (player)
				printf("%d %s %.2f\n", player->id, player->name, player->averageRating);

			idList = idList->next;
		}
	}
}
