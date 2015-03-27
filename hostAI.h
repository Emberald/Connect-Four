//
//  hostAI.h
//  Connect Four
//
//  Created by Simon on 3/17/15.
//  Copyright (c) 2015 simon. All rights reserved.
//

#ifndef __Connect_Four__hostAI__
#define __Connect_Four__hostAI__

#include "interface.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define FIRST_MOVE 0
#define SECOND_MOVE 1
#define EMPTY_SLOT -1
#define isColumnFull(board, lastMove) board[lastMove][0] != EMPTY_SLOT
#define TIE -50
#define winEins 100
#define winBohr -100
typedef __int64_t Bitboard;
typedef struct {Bitboard einsBoard; Bitboard bohrBoard; int lastColumn; int lastRow;} Board;

int predictor(int **board, int depth, double best, double worst, int lastMove);
int evaluator(int **board);
int chkColHeight(int **board, int lastMove);
int winnerChecker(int **board, int lastMove);
int updateBoard(int **board, int last_move, int player_num);


#endif /* defined(__Connect_Four__hostAI__) */
