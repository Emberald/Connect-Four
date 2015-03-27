//
//  guestAI.c
//  Connect Four
//
//  Created by Simon on 3/17/15.
//  Copyright (c) 2015 simon. All rights reserved.
//

#include "guestAI.h"

static PlayerSetup guest_store_setup;
static int *sample_board;
static int guest_flag_initialized = 0;

void guest_setup(PlayerSetup setup) {
    guest_store_setup = setup;
    if (guest_flag_initialized == 0) {
        sample_board = calloc(setup.board_size_horiz, sizeof(int));
        guest_flag_initialized = 1;
    }
    if (guest_store_setup.counter_game == 1)
        srand((unsigned) time(NULL));

    printf("...\n[Guest AI Initialized] \n");
    printf("\"Good luck and have Fun!\" \n\n");
}

int guest_move(int *move_history, int counter_move) {
    for (int i = 0; i < guest_store_setup.board_size_horiz; i++)
        sample_board[i] = 0;
    for (int i = 0; i < counter_move; i++)
        sample_board[move_history[i]]++;

    int move;
    do {
        move = rand() % guest_store_setup.board_size_horiz;
    } while (sample_board[move] >= guest_store_setup.board_size_vert);
    return move;
}

void guest_reset() {
    if (guest_flag_initialized == 1) {
        free(sample_board);
        guest_flag_initialized = 0;
    }
}
