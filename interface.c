#include "conio2.h"
#include <stdio.h>
#include <windows.h>
#include "fifadb.h"

// -------------------------// Bibliotecas //------------------------- //
#include <conio2.h>
#include <stdio.h>
#include <windows.h>

#include "fifadb.h"


// remove o cursor da tela
void remove_cursor(){
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}


// -> SET_CURSOR: coloca o cursor na tela
void set_cursor(){
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 5;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
}


// printa uma tela com informacoes iniciais
void initial_screen(){


    // Imprime as informacoes
    textcolor(YELLOW);
    gotoxy(77, 20);
    printf("CLASSIFICACAO E PESQUISA DE DADOS");
    textcolor(WHITE);
    gotoxy(78, 22);
    printf("Trabalho final");
    gotoxy(100, 29);
    printf("Eduardo Wullner");
    gotoxy(100,30);
    printf("Gabriel Tavares");


    getchar();
}


// imprime um retangulo
void print_rectangle(){
    int y;

    clrscr();

    textbackground(YELLOW);
    gotoxy(50, 9);
    printf("                                                                                  ");

    for(y = 10; y < 42; y++){
        gotoxy(50, y);
        printf(" ");

        gotoxy(131, y);
        printf(" ");
    }

    gotoxy(50, 42);
    printf("                                                                                  ");
    textbackground(BLACK);
}


// constroi o retangulo onde as informacoes sobre a construcao das estruturas serao impressas
void construction_screen(){
    print_rectangle();

    textcolor(YELLOW);
    gotoxy(55, 13);
    printf("CONSTRUCAO DAS ESTRUTURAS");
    textcolor(WHITE);
}


// constroi o retangulo com as possiveis escolhas de consulta
void options_query(){
    print_rectangle();

    textcolor(YELLOW);
    gotoxy(55, 13);
    printf("CONSULTAS");
    textcolor(WHITE);
    gotoxy(55, 15);
    printf("Selecione o que deseja consultar");

    gotoxy(58, 21);
    printf("Pesquisa de jogadores por prefixo");
    gotoxy(58, 23);
    printf("Avaliacao de jogadores por usuario");
    gotoxy(58, 25);
    printf("Top jogadores de uma posicao");
    gotoxy(58, 27);
    printf("Tags associadas a um jogador");
    gotoxy(58, 29);
    printf("Sair");
}


// impirme as setinhas que indicam em qual opcao o usuario esta no momento
void print_selection(int option){
    textcolor(YELLOW);
    switch(option){
        case 1:
            gotoxy(56, 21);
            break;
        case 2:
            gotoxy(56, 23);
            break;
        case 3:
            gotoxy(56, 25);
            break;
        case 4:
            gotoxy(56, 27);
            break;
        case 5:
            gotoxy(56, 29);
            break;
    }

    printf(">");
    textcolor(WHITE);
}


// apaga as setinhas da posicao anterior que o usuario estava
void del_selection(int option){
    switch(option){
        case 1:
            gotoxy(56, 21);
            break;
        case 2:
            gotoxy(56, 23);
            break;
        case 3:
            gotoxy(56, 25);
            break;
        case 4:
            gotoxy(56, 27);
            break;
        case 5:
            gotoxy(56, 29);
            break;
    }

    printf(" ");
}


// controla a animacao de escolha de opcoes do jogador e devolve a opcao escolhida
int select_option(){
    char move;

    int flag = 0;
    int option = 1;

    print_selection(option);

    do{
        move = getch();
        switch(move){
            case 72:
                if(option == 1){
                    del_selection(option);
                    option = 5;
                    print_selection(option);
                } else {
                    del_selection(option);
                    option--;
                    print_selection(option);
                }

                break;
            case 80:
                if(option == 5){
                    del_selection(option);
                    option = 1;
                    print_selection(option);
                } else {
                    del_selection(option);
                    option++;
                    print_selection(option);
                }

                break;
            case 13:
                del_selection(option);
                flag = 1;
                break;
        }
    } while(flag == 0);

    return option;
}


// controla a tela de consulta
int query_screen(){
    int option;

    clrscr();
    options_query();
    option = select_option();

    return option;
}


