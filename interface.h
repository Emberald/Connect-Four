//
//  interface.h
//  Connect Four
//
//  Created by Simon on 3/18/15.
//  Copyright (c) 2015 simon. All rights reserved.
//

#ifndef Connect_Four_interface_h
#define Connect_Four_interface_h

#define GAME_TIE 2
#define GAME_CONTINUE 9
#define GAME_RESTART 10
typedef enum {null = -1, Tester, Beginner, Easy, Normal, Hard, Insane} Difficulty;

typedef struct {
    int counter_game;
    int board_size_horiz;
    int board_size_vert;
    int piece_in_row;
    Difficulty difficulty;
} PlayerSetup;

typedef struct {
    void (*setup)();
    int (*move)();
    void (*reset)();
} Interface;

void guest_setup(PlayerSetup setup);
int guest_move(int *move_history, int counter_move);
void guest_reset();

static const Interface guest_AI = {
    guest_setup,
    guest_move,
    guest_reset
};

void host_setup(PlayerSetup setup);
int host_move(int *move_history, int counter_move);
void host_reset();

static const Interface host_AI = {
    host_setup,
    host_move,
    host_reset
};

void human_setup(PlayerSetup setup);
int human_move(int *move_history, int counter_move);
void human_reset();

static const Interface human = {
    human_setup,
    human_move,
    human_reset
};

#endif
