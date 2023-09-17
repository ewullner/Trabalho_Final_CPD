#include "fifadb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX 500

int main(int argc, char** argv) {
    remove_cursor();

     // --> INTERFACE INICIAL
    initial_screen();


    // --> CONSTRUCAO DAS ESTRUTURAS
    // Interface da construcao das estruturas
    construction_screen();

	// -----------------------------------------------------------------
	// Arquivos para teste
	FILE* players_csv = NULL, * rating_csv = NULL, * tags_csv = NULL;

	players_csv = fopen("C:\\Users\\gabri\\OneDrive\\Documentos\\UFRGS\\3semestre\\CPD\\Trabalho_Final\\Entradas\\players.csv", "r");
	textcolor(LIGHTGRAY);
        gotoxy(55, 18);
        printf("Arquivo players aberto com sucesso");
        textcolor(WHITE);
	rating_csv = fopen("C:\\Users\\gabri\\OneDrive\\Documentos\\UFRGS\\3semestre\\CPD\\Trabalho_Final\\Entradas\\rating.csv", "r");
	 textcolor(LIGHTGRAY);
        gotoxy(55, 19);
        printf("Arquivo ratings aberto com sucesso");
        textcolor(WHITE);
	tags_csv = fopen("C:\\Users\\gabri\\OneDrive\\Documentos\\UFRGS\\3semestre\\CPD\\Trabalho_Final\\Entradas\\tags.csv", "r");
	 textcolor(LIGHTGRAY);
        gotoxy(55, 20);
        printf("Arquivo tags aberto com sucesso");
        textcolor(WHITE);

	// -----------------------------------------------------------------
	// Estruturas de dados
	PlayerHash playerHash;
	UserHash userHash;
	TagHash tagHash;
	PositionList playersByPos;
	Trie trie;

    // -----------------------------------------------------------------
	// Leitura de arquivos e construção das estruturas de dados
    savePlayerData(players_csv, &playerHash);
     textcolor(LIGHTGRAY);
        gotoxy(55, 22);
        printf("Dados dos jogadores salvos com sucesso");
        textcolor(WHITE);
	saveRatings(rating_csv, &playerHash, &userHash);
	textcolor(LIGHTGRAY);
        gotoxy(55, 23);
        printf("Ratings dos usuarios salvos com sucesso");
        textcolor(WHITE);
	saveTags(tags_csv, &tagHash);
    textcolor(LIGHTGRAY);
        gotoxy(55, 24);
        printf("Tags dos jogadores salvas com sucesso");
        textcolor(WHITE);
	groupPlayersByPosition(&playersByPos, playerHash);
	makePlayerNameTrie(&trie, playerHash);


	int32_t flag = 0;
    int32_t number;
    int numb;

    do{
      // -----------------------------------------------------------------
	  // Inicialização da interface de escolha de operação:

      number = query_screen();
      clrscr();
      set_cursor();

      if(number == 1){
            // -----------------------------------------------------------------
	        // Pesquisas sobre os nomes de jogadores

             printf("\nInsira o prefixo do jogador:\n");
             char* playerName1[MAX];
             fflush(stdin);
             scanf("%s", &playerName1);
             fflush(stdin);
	         printf("\nPesquisa %s:\n", playerName1);
             remove_cursor();
	         wildcardQuery(&trie, playerHash, playerName1);
	         system("pause");
      }
            // -----------------------------------------------------------------
	        // Pesquisas sobre jogadores revisados por usuários
      else if (number == 2){
             printf("Insira o userId do usuario:\n");
             int32_t user1;
             scanf("%d", &user1);
             fflush(stdin);
             printf("\nPesquisa userId = %d\n", user1);
             remove_cursor();
	         showUserRatings(user1, &userHash, playerHash);
	         system("pause");
      }
            // -----------------------------------------------------------------
	        // Pesquisas sobre os melhores jogadores de uma determinada posição
      else if (number == 3){
             printf("Insira a posicao do jogador:\n");
             char* pos1[MAX];
             int32_t N = 10;
             fflush(stdin);
             scanf("%s", &pos1);
             fflush(stdin);
             printf("\nPesquisa %s:\n", pos1);
             remove_cursor();
             showTopPlayers(playersByPos, pos1, N);
             system("pause");
      }
            // -----------------------------------------------------------------
	        // Pesquisas sobre ‘tags’ de jogadores
      else if(number == 4){
             printf("Insira quantas tags voce deseja procurar \n");
             scanf("%d", &numb);
             char* tags[numb];
             fflush(stdin);
	         char* tag[MAX];
            for(int i=0; i<numb; i++){
              printf("Insira uma tag que voce deseja buscar: \n");
               fflush(stdin);
               gets(tag);
               tags[i] = strdup(tag);
            }
            fflush(stdin);
	         printf("\nPesquisa por tags: ");
             printf("\n");
	         tagQuery(tags, numb, &tagHash, playerHash);
             printf("\n");
             system("pause");
      }
      else if(number == 5)
             flag = 1;

    }while(flag == 0);

	// -----------------------------------------------------------------
	system("pause");
	return 0;
}
