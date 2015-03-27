//
//  humanPlayer.c
//  Connect Four
//
//  Created by Simon on 3/17/15.
//  Copyright (c) 2015 simon. All rights reserved.
//

#include "humanPlayer.h"

static PlayerSetup human_store_setup;
static int *human_board;
static int human_flag_initialized = 0;

void human_setup(PlayerSetup setup) {
    human_store_setup = setup;
    if (human_flag_initialized == 0) {
        human_board = calloc(setup.board_size_horiz, sizeof(int));
        human_flag_initialized = 1;
    }
}

int human_move(int *move_history, int counter_move) {
    for (int i = 0; i < human_store_setup.board_size_horiz; i++)
        human_board[i] = 0;
    for (int i = 0; i < counter_move; i++)
        human_board[move_history[i]]++;
    
    while (1) {
        int error_flag = 0;
        char input;
        static int mark = 0;
        if ((human_store_setup.board_size_horiz >= 25 || human_store_setup.board_size_vert >= 20) && mark++ == 0)
            printf("Tips: enter Q to restart. \n");
        printf("> Please enter your move: ");
        while ((input = getchar()) == '\n' || input == ' ');
        while (getchar() != '\n')
            error_flag = 1;
        switch (input) {
            case 'q':
            case 'Q':
                return GAME_RESTART;
            default:
                break;
        }
        input -= '1';
        if (error_flag || input < 0 || input >= human_store_setup.board_size_horiz)
            printf("Not a valid move. \n");
        else if (human_board[input] >= human_store_setup.board_size_vert)
            printf("This column is full. \n");
        else
            return input;
    }
}

void human_reset() {
    if (human_flag_initialized == 1) {
        free(human_board);
        human_flag_initialized = 0;
    }
}
