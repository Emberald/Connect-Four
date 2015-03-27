//
//  hostAI.c
//  Connect Four
//
//  Created by Simon on 3/17/15.
//  Copyright (c) 2015 simon. All rights reserved.
//

#include "hostAI.h"

static PlayerSetup hostSetup;

void host_setup(PlayerSetup setup) {
    hostSetup = setup;
    if (hostSetup.counter_game == 1)
        srand((unsigned) time(NULL));
}

int host_move(int *move_history, int counter_move) {
    static int timeCoef[3] = {0};//
    int countTurn = counter_move / 2 + 1, computer_num = counter_move % 2;
    if (countTurn == 1) {
        srand((unsigned) time(NULL));
        timeCoef[0] = 0;//
        timeCoef[1] = 0;
        timeCoef[2] = 0;
    }
    time_t start = clock();
    // preset opening
    int move = hostSetup.board_size_horiz;
    if (counter_move == 0 || counter_move == 1)
        move = hostSetup.board_size_horiz / 2;
    else if (counter_move == 2 && move_history[2] == hostSetup.board_size_horiz / 2)
        move = hostSetup.board_size_horiz / 2;
    else {
        // initialize board
        static int **board;
        static int flagInitialized = 0;
        if (counter_move == 2 || counter_move == 3) {
            if (flagInitialized == 1) {
                for (int i = 0; i < hostSetup.board_size_horiz; i++)
                    free(board[i]);
                free(board);
                flagInitialized = 0;
            }
            
            board = malloc(hostSetup.board_size_horiz * sizeof(int *));
            if(board == NULL)
                exit(1);
            for(int i = 0; i < hostSetup.board_size_horiz; i++) {
                board[i] = malloc(hostSetup.board_size_vert * sizeof(int));
                if(board[i] == NULL)
                    exit(1);
            }
            flagInitialized = 1;
        }
        for (int i = 0; i < hostSetup.board_size_horiz; i++)
            for (int j = 0; j < hostSetup.board_size_vert; j++)
                board[i][j] = EMPTY_SLOT;
        if (computer_num == FIRST_MOVE)
            for (int i = 0; i < counter_move; i++)
                updateBoard(board, move_history[i], i % 2);
        else
            for (int i = 0; i < counter_move; i++)
                updateBoard(board, move_history[i], !(i % 2));
        // initialization
        int localDifficulty = hostSetup.difficulty + timeCoef[computer_num];
        int depth = localDifficulty * 2 - 1, maxValue = winBohr - depth - 1, minValue = -maxValue, keeperRow, lastChosenColumn = -1, columnCheck = (hostSetup.board_size_horiz / 2.0) + 0.5;//if board is symmetric, check left half columns only
        for (int i = 0; i < hostSetup.board_size_horiz / 2; i++) {
            for (int j = hostSetup.board_size_vert - 1; j >= 0; j--) {
                if (board[i][j] == 0 && board[hostSetup.board_size_horiz - i - 1][j] == 0)
                    break;
                else if (board[i][j] != board[hostSetup.board_size_horiz - i - 1][j]) {
                    columnCheck = hostSetup.board_size_horiz;
                    break;
                }
            }
            if (columnCheck == hostSetup.board_size_horiz)
                break;
        }
        // Basic check
        if (localDifficulty > Tester) {
            int keeperScore;
            for (int i = 0; i < columnCheck; i++) { //if going this way wins, do it
                if (isColumnFull(board, i))
                    continue;
                keeperRow = updateBoard(board, i, FIRST_MOVE);
                keeperScore = winnerChecker(board, i);
                board[i][keeperRow] = EMPTY_SLOT;
                if (keeperScore >= winEins)
                    return i;
            }
            for (int i = 0; i < columnCheck; i++) { //if don't go this way loses, do it
                if (isColumnFull(board, i))
                    continue;
                keeperRow = updateBoard(board, i, SECOND_MOVE);
                keeperScore = winnerChecker(board, i);
                board[i][keeperRow] = EMPTY_SLOT;
                if (keeperScore <= winBohr)
                    return i;
            }
        }
        // shallow check, for Hard and Insane
        if (localDifficulty >= Hard) {
            int keeperScore;
            for (int i = 0; i < columnCheck; i++) {
                if (isColumnFull(board, i))
                    continue;
                keeperRow = updateBoard(board, i, FIRST_MOVE);
                keeperScore = predictor(board, 3, maxValue, minValue, i);
                board[i][keeperRow] = EMPTY_SLOT;
                if (keeperScore >= winEins)
                    return i;
            }
        }
        // deep check
//        printf("counter_move:%d\n", counter_move);
        if (localDifficulty >= Beginner) {
//            printf("depth:%d\n", localDifficulty * 2 - 1);//
            int keeperScore[hostSetup.board_size_horiz], maxScore = maxValue;
            for (int i = 0; i < columnCheck; i++) {
                if (isColumnFull(board, i))
                    continue;
                keeperRow = updateBoard(board, i, FIRST_MOVE);
                keeperScore[i] = predictor(board, depth, maxValue, minValue, i);
                board[i][keeperRow] = EMPTY_SLOT;
//                printf("%d,%d\n", i, keeperScore[i]);//
                if (keeperScore[i] > maxScore) {
                    maxScore = keeperScore[i];
                    move = i;
                    lastChosenColumn = i;
                }
                else if (keeperScore[i] == maxScore && abs(i - hostSetup.board_size_horiz / 2) < abs(lastChosenColumn - hostSetup.board_size_horiz / 2)) {
                    move = i;
                    lastChosenColumn = i;
                }
            }
        }
    }
    time_t end = clock();
    double timeSpent = (double) (winEins * (end - start) / CLOCKS_PER_SEC);
//    printf("%.0lfms\n", timeSpent);//
    if (timeSpent < 600 && countTurn > 3 && hostSetup.difficulty >= Insane) {
        timeCoef[computer_num]++;
    }
    if (move != hostSetup.board_size_horiz)
        return move;
    else
        return -1;
}

void host_reset() {

}

int predictor(int **board, int depth, double best, double worst, int lastMove)
{
    if (winnerChecker(board, lastMove) == winEins)
        return (winEins + depth) * ((depth % 2 == 1) ? 1 : -1);
    if (depth <= 1)
        return evaluator(board);
    
    int returnResult, keeperRow, mark = 0;
    if (depth % 2 == 0) {
        for (int i = 0; i < hostSetup.board_size_horiz; i++) {
            if (isColumnFull(board, i))
                continue;
            mark = 1;
            keeperRow = updateBoard(board, i, FIRST_MOVE);
            returnResult = predictor(board, depth - 1, best, worst, i);
            board[i][keeperRow] = EMPTY_SLOT;
            if (returnResult > best)
                best = returnResult;
            if (worst <= best)
                break;
        }
        if (mark)
            return best;
    }
    else {
        for (int i = 0; i < hostSetup.board_size_horiz; i++) {
            if (isColumnFull(board, i))
                continue;
            mark = 1;
            keeperRow = updateBoard(board, i, SECOND_MOVE);
            returnResult = predictor(board, depth - 1, best, worst, i);
            board[i][keeperRow] = EMPTY_SLOT;
            if (returnResult < worst)
                worst = returnResult;
            if (worst <= best)
                break;
        }
        if (mark)
            return worst;
    }
    return TIE;
}

int evaluator(int **board) //OLD PREDICTOR
{
    int keeperRow, posBohr = 0, posEins = 0, combo = 0, check;
    for (int i = 0; i < hostSetup.board_size_horiz; i++) {
        if (isColumnFull(board, i))
            continue;
        posEins++;
        if (combo > 1)
            continue;
        keeperRow = updateBoard(board, i, 1);
        check = winnerChecker(board, i);
        board[i][keeperRow] = EMPTY_SLOT;
        if (check && combo)
            combo++;
        else if (check || combo) {
            posBohr = 1;
            combo = 1;
        }
        else
            posBohr++;
    }
    if (combo > 1)
        return posEins;
    combo = 0;
    for (int i = 0; i < hostSetup.board_size_horiz; i++) {
        if (isColumnFull(board, i))
            continue;
        keeperRow = updateBoard(board, i, 2);
        check = winnerChecker(board, i);
        board[i][keeperRow] = EMPTY_SLOT;
        if (check && combo) {
            posEins = 0;
            break;
        }
        else if (check || combo) {
            posEins = 1;
            combo = 1;
        }
    }
    return posEins - posBohr;
}

int winnerChecker(int **board, int lastMove)
{
    int lastMove_vertical;
    for (int i = 0; i < hostSetup.board_size_vert; i++)
        if (board[lastMove][i] != EMPTY_SLOT) {
            lastMove_vertical = i; //check column height
            break;
        }
    int count, lastMoveCol, lastMoveRow, numPlayer = board[lastMove][lastMove_vertical];
    count = 1;
    lastMoveCol = lastMove + 1;
    while (lastMoveCol < hostSetup.board_size_horiz && board[lastMoveCol][lastMove_vertical] == numPlayer) {
        lastMoveCol++;
        count++;
    }
    lastMoveCol = lastMove - 1;
    while (lastMoveCol >= 0 && board[lastMoveCol][lastMove_vertical] == numPlayer) {
        lastMoveCol--;
        count++;
    }
    if (count >= hostSetup.piece_in_row)
        return winEins;
    //            check | and 3|
    count = 1;
    lastMoveRow = lastMove_vertical + 1;
    while (lastMoveRow < hostSetup.board_size_vert && board[lastMove][lastMoveRow] == numPlayer) {
        lastMoveRow++;
        count++;
    }
    if (count >= hostSetup.piece_in_row)
        return winEins;
    //    check \ and \3
    count = 1;
    lastMoveCol = lastMove + 1;
    lastMoveRow = lastMove_vertical + 1;
    while (lastMoveCol < hostSetup.board_size_horiz && lastMoveRow < hostSetup.board_size_vert && board[lastMoveCol][lastMoveRow] == numPlayer) {
        lastMoveCol++;
        lastMoveRow++;
        count++;
    }
    lastMoveCol = lastMove - 1;
    lastMoveRow = lastMove_vertical - 1;
    while (lastMoveCol >= 0 && lastMoveRow >= 0 && board[lastMoveCol][lastMoveRow] == numPlayer) {
        lastMoveCol--;
        lastMoveRow--;
        count++;
    }
    if (count >= hostSetup.piece_in_row)
        return winEins;
    //          check / and /3
    count = 1;
    lastMoveCol = lastMove + 1;
    lastMoveRow = lastMove_vertical - 1;
    while (lastMoveCol < hostSetup.board_size_horiz && lastMoveRow >= 0 && board[lastMoveCol][lastMoveRow] == numPlayer) {
        lastMoveCol++;
        lastMoveRow--;
        count++;
    }
    lastMoveCol = lastMove - 1;
    lastMoveRow = lastMove_vertical + 1;
    while (lastMoveCol >= 0 && lastMoveRow < hostSetup.board_size_vert && board[lastMoveCol][lastMoveRow] == numPlayer) {
        lastMoveCol--;
        lastMoveRow++;
        count++;
    }
    if (count >= hostSetup.piece_in_row)
        return winEins;
    return 0;
}

int updateBoard(int **board, int last_move, int numPlayer)
{
    for (int i = hostSetup.board_size_vert - 1; i >= 0; i--)
        if (board[last_move][i] == EMPTY_SLOT) {
            board[last_move][i] = numPlayer;
            return i;
        }
    return -1;
}
