//
//  main.h
//  Connect Four
//
//  Created by Simon on 3/17/15.
//  Copyright (c) 2015 simon. All rights reserved.
//

#ifndef Connect_Four_main_h
#define Connect_Four_main_h

#include "interface.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define FIRST_MOVE 0
#define SECOND_MOVE 1
#define EMPTY_SLOT -1
#define ROLE_GUEST 1
#define ROLE_HOST 2
#define ROLE_HUMAN 0
#define ERROR_ROLE 1
#define ERROR_MEMORY 2
#define ERROR_MOVE 3

typedef struct {
    int role_player_X;
    int role_player_O;
    Difficulty difficulty_player_X;
    Difficulty difficulty_player_O;
    char title_player_X[20];
    char title_player_O[20];
    int board_size_horiz;
    int board_size_vert;
    int board_size;
    int piece_in_row;
} GlobalSetup;

typedef struct {
    int **matrix;
    int last_move;
    int *move_histogram;
    int *move_history;
    int counter_move;
} Board;

void setup_global(GlobalSetup *setup);
void setup_player(GlobalSetup *setup, PlayerSetup *setup_X, PlayerSetup *setup_O, Interface *player_X, Interface *player_O);
char display_control_panel(GlobalSetup *setup);
void switch_player(int *role_player);
void set_difficulty(Difficulty *difficulty);
char *get_difficulty(Difficulty difficulty);
void set_title(GlobalSetup *setup);
void set_board_size(GlobalSetup *setup);
void set_winning_count(GlobalSetup *setup);
void initialize_board(Board *board, GlobalSetup *setup);
void update_board(Board *board, int last_move, GlobalSetup *setup);
void check_move_validity(Board *board, int last_move, GlobalSetup *setup);
void display_board(Board *board, GlobalSetup *setup);
int check_win_or_tie(Board *board, GlobalSetup *setup);
int check_winner(Board *board, GlobalSetup *setup);

static char* string_beginner = "Beginner";
static char* string_easy = "Easy";
static char* string_normal = "Normal";
static char* string_hard = "Hard";
static char* string_insane = "Insane";
static char* string_NA = "N/A";


#endif
