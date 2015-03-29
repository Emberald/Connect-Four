//
//  main.c
//  Connect Four
//
//  Created by Simon on 3/17/15.
//  Copyright (c) 2015 simon. All rights reserved.
//

#include "main.h"

int main()
{
    printf("*******************************************\n");
    printf("**  Welcome to the Connect-Four game!!!  **\n");
    printf("*******************************************\n\n");
    //initialization
    static GlobalSetup setup = {
        .role_player_X = ROLE_HUMAN,
        .role_player_O = ROLE_HOST,
        .difficulty_player_X = Insane,
        .difficulty_player_O = Insane,
        .piece_in_row = 4,
        .board_size_horiz = 7,
        .board_size_vert = 6,
        .board_size = 42
    };
    static Interface player_X, player_O;
    static PlayerSetup setup_X = { .counter_game = 0 }, setup_O = { .counter_game = 0 };
    static Board board;
    void *copy_move_history;

    //game loop
    while (1) {
        // pre-game
        int last_move = 0;
        setup_global(&setup);
        setup_player(&setup, &setup_X, &setup_O, &player_X, &player_O);
        initialize_board(&board, &setup);
        display_board(&board, &setup);
        (*player_X.setup)(setup_X);
        (*player_O.setup)(setup_O);
        
        // round loop
        while (1) {
            // player X move
            last_move = (*player_X.move)(memcpy(copy_move_history, board.move_history, setup.board_size * sizeof(int)), board.counter_move);
            if (last_move == GAME_RESTART)
                break;
            printf("%s moved in column: %c\n", setup.title_player_X, last_move + '1');
            check_move_validity(&board, last_move, &setup);
            update_board(&board, last_move, &setup);
            display_board(&board, &setup);
            // check winner
            if (check_win_or_tie(&board, &setup) == GAME_RESTART)
                break;
            
            // player O move
            last_move = (*player_O.move)(memcpy(copy_move_history, board.move_history, setup.board_size * sizeof(int)), board.counter_move);
            if (last_move == GAME_RESTART)
                break;
            printf("%s moved in column: %c\n", setup.title_player_O, last_move + '1');
            check_move_validity(&board, last_move, &setup);
            update_board(&board, last_move, &setup);
            display_board(&board, &setup);
            // check winner
            if (check_win_or_tie(&board, &setup) == GAME_RESTART)
                break;
        }
        
        // post-game
        (*player_X.reset)();
        (*player_O.reset)();
        for (int i = 0; i < 8; i++)
            printf("> \n");
        printf("\n");
    }
}

void setup_global(GlobalSetup *setup)
{
    while (1) {
        set_title(setup);
        char choice = display_control_panel(setup);
        switch (choice) {
            case '1':
                set_board_size(setup);
                break;
            case '2':
                set_winning_count(setup);
                break;
            case '3':
                switch_player(&setup->role_player_X);
                break;
            case '4':
                switch_player(&setup->role_player_O);
                break;
            case '5':
                printf("\n");
                return;
            case 'x':
                set_difficulty(&setup->difficulty_player_X);
                break;
            case 'o':
                set_difficulty(&setup->difficulty_player_O);
                break;
            default:
                printf("Not a valid input. \n");
                break;
        }
        printf("\n");
    }
}

void setup_player(GlobalSetup *setup, PlayerSetup *setup_X, PlayerSetup *setup_O, Interface *player_X, Interface *player_O)
{
    setup_X->counter_game++;
    setup_X->board_size_horiz = setup->board_size_horiz;
    setup_X->board_size_vert = setup->board_size_vert;
    setup_X->piece_in_row = setup->piece_in_row;
    setup_X->difficulty = setup->difficulty_player_X;
    setup_O->counter_game++;
    setup_O->board_size_horiz = setup->board_size_horiz;
    setup_O->board_size_vert = setup->board_size_vert;
    setup_O->piece_in_row = setup->piece_in_row;
    setup_O->difficulty = setup->difficulty_player_O;
    
    switch (setup->role_player_X) {
        case ROLE_GUEST:
            *player_X = guest_AI;
            break;
        case ROLE_HOST:
            *player_X = host_AI;
            break;
        case ROLE_HUMAN:
            *player_X = human;
            break;
        default:
            break;
    }
    switch (setup->role_player_O) {
        case ROLE_GUEST:
            *player_O = guest_AI;
            break;
        case ROLE_HOST:
            *player_O = host_AI;
            break;
        case ROLE_HUMAN:
            *player_O = human;
            break;
        default:
            break;
    }
}

char display_control_panel(GlobalSetup *setup)
{
    char input;
    int sequence = 0;
    
    printf("Control Panel \n");
    printf("%d - Size of board ........... [%d columns x %d rows] \n", ++sequence, setup->board_size_horiz, setup->board_size_vert);
    printf("%d - Number of pieces in a row [%d piece%s] \n", ++sequence, setup->piece_in_row, (setup->piece_in_row == 1) ? "" : "s");
    printf("%d - Player X ................ [%s] \n", ++sequence, setup->title_player_X);
    if (setup->role_player_X != ROLE_HUMAN)
        printf("  X - Difficulty [%s] \n", get_difficulty(setup->difficulty_player_X));
    printf("%d - Player O ................ [%s] \n", ++sequence, setup->title_player_O);
    if (setup->role_player_O != ROLE_HUMAN)
        printf("  O - Difficulty [%s] \n", get_difficulty(setup->difficulty_player_O));
    printf("%d - Nothing ................. [Go!] \n", ++sequence);

    while (1) {
        printf("> Which setting would you like to change? [1-5%s%s]: ", (setup->role_player_X != ROLE_HUMAN) ? ",X" : "", (setup->role_player_O != ROLE_HUMAN) ? ",O" : "");
        while ((input = getchar()) == '\n' || input == ' ');
        while (getchar() != '\n');
        if (input <= sequence + '0' && input > '0')
            break;
        else if (setup->role_player_X != ROLE_HUMAN && (input == 'X' || input == 'x')) {
            if (input == 'X')
                input = 'x';
            break;
        }
        else if (setup->role_player_O != ROLE_HUMAN && (input == 'O' || input == 'o')) {
            if (input == 'O')
                input = 'o';
            break;
        }
        else
            printf("Not a valid input. Please try again. \n");
    }
    return input;
}

void switch_player(int *role_player)
{
    if (*role_player == 2)
        *role_player = 0;
    else
        (*role_player)++;
}

void set_difficulty(Difficulty *difficulty)
{
    char input;
    while (1) {
        printf("> Please enter difficulty level. [b/e/n/h/i]: ");
        while ((input = getchar()) == '\n' || input == ' ');
        while (getchar() != '\n');
        switch (input) {
            case 'B':
            case 'b':
                *difficulty = Beginner;
                return;
            case 'E':
            case 'e':
                *difficulty = Easy;
                return;
            case 'N':
            case 'n':
                *difficulty = Normal;
                return;
            case 'H':
            case 'h':
                *difficulty = Hard;
                return;
            case 'I':
            case 'i':
                *difficulty = Insane;
                return;
            default:
                printf("Not a valid input. \n");
                break;
        }
    }
}

char *get_difficulty(Difficulty difficulty)
{
    char *strDifficult;
    switch (difficulty) {
        case Beginner:
            strDifficult = string_beginner;
            break;
        case Easy:
            strDifficult = string_easy;
            break;
        case Normal:
            strDifficult = string_normal;
            break;
        case Hard:
            strDifficult = string_hard;
            break;
        case Insane:
            strDifficult = string_insane;
            break;
        default:
            strDifficult = string_NA;
            break;
    }
    return strDifficult;
}

void set_board_size(GlobalSetup *setup)
{
    int column, row;
    while (1) {
        printf("> Please enter numbers of column and row, seperated by a space. [1-78]: ");
        scanf("%d %d", &column, &row);
        while (getchar() != '\n');
        if (column  > 0 && row  > 0 && column  <= 78 && row  <= 78) {
            if (column > 8) {
                char input;
                printf("> Large board may result in serious lags when applying host AI with high difficulty. Are you sure to proceed? [y/n]: \n");
                if ((input = getchar()) != 'y')
                    continue;
            }
            break;
        }
        else
            printf("Not a valid input. \n");
    }
    setup->board_size_vert = row;
    setup->board_size_horiz = column;
    setup->board_size = row * column;
}

void set_winning_count(GlobalSetup *setup)
{
    int input;
    while (1) {
        printf("> Please enter a positive number: ");
        scanf("%d", &input);
        while (getchar() != '\n');
        if (input > 0) {
            setup->piece_in_row = input;
            return;
        }
        else
            printf("Not a valid input. \n");
    }
}

void set_title(GlobalSetup *setup)
{
    switch (setup->role_player_X) {
        case ROLE_GUEST:
            strcpy(setup->title_player_X, "Guest AI");
            break;
        case ROLE_HOST:
            strcpy(setup->title_player_X, "Host AI");
            break;
        case ROLE_HUMAN:
            strcpy(setup->title_player_X, "Player");
            break;
        default:
            break;
    }
    switch (setup->role_player_O) {
        case ROLE_GUEST:
            strcpy(setup->title_player_O, "Guest AI");
            break;
        case ROLE_HOST:
            strcpy(setup->title_player_O, "Host AI");
            break;
        case ROLE_HUMAN:
            strcpy(setup->title_player_O, "Player");
            break;
        default:
            break;
    }
    if (setup->role_player_X == setup->role_player_O) {
        strcat(setup->title_player_X, " #1");
        strcat(setup->title_player_O, " #2");
    }
}

void initialize_board(Board *board, GlobalSetup *setup)
{
    if (board->counter_move != 0) {
        for (int i = 0; i < setup->board_size_horiz; i++)
            free(board->matrix[i]);
        free(board->matrix);
        free(board->move_histogram);
        free(board->move_history);
    }
    
    board->matrix = malloc(setup->board_size_horiz * sizeof(int *));
    if(board->matrix == NULL)
        exit(ERROR_MEMORY);
    for(int i = 0; i < setup->board_size_horiz; i++) {
        board->matrix[i] = malloc(setup->board_size_vert * sizeof(int));
        if(board->matrix[i] == NULL)
            exit(ERROR_MEMORY);
        for (int j = 0; j < setup->board_size_vert; j++)
            board->matrix[i][j] = EMPTY_SLOT;
    }
    board->last_move = -1;
    board->move_histogram = calloc(setup->board_size_horiz, sizeof(int));
    board->move_history = malloc(setup->board_size * sizeof(int));
    for (int i = 0; i < setup->board_size; i++)
        board->move_history[i] = -1;
    board->counter_move = 0;
}

void check_move_validity(Board *board, int last_move, GlobalSetup *setup)
{
    if (last_move < 0 || last_move >= setup->board_size_horiz || board->move_histogram[last_move] >= setup->board_size_vert) {
        printf("%s moved wrongly! \n> Press enter to exit. ", (board->counter_move % 2 == 0) ? setup->title_player_X : setup->title_player_O);//
        getchar();
        exit(ERROR_MOVE);
    }
}

void display_board(Board *board, GlobalSetup *setup)
{
    for (int i = setup->board_size_vert - 1; i >= -1; i--) {
        for (int j = 0; j < setup->board_size_horiz; j++)
            printf("+---");
        printf("+\n");
        for (int k = 0; k < setup->board_size_horiz; k++) {
            if (i >= 0) {
                if (k == board->last_move && i == board->move_histogram[board->last_move] - 1)
                    printf("|%s", (board->matrix[k][i] == FIRST_MOVE) ? "_X_" : "_O_");
                else
                    printf("|%s", (board->matrix[k][i] != EMPTY_SLOT) ? ((board->matrix[k][i] == FIRST_MOVE) ? " X " : " O ") : "   ");
            }
            else
                printf("  %c ", k + '1');
        }
        if (i >= 0)
            printf("|\n");
    }
    printf("\n\n");
}

int check_win_or_tie(Board *board, GlobalSetup *setup)
{
    static int counter_win[3] = {0};

    switch (check_winner(board, setup)) {
        case FIRST_MOVE:
            printf("*****************\n* Player X won! *\n*****************\n");
            if (setup->role_player_X != setup->role_player_O)
                counter_win[setup->role_player_X]++;
            break;
        case SECOND_MOVE:
            printf("*****************\n* Player O won! *\n*****************\n");
            if (setup->role_player_X != setup->role_player_O)
                counter_win[setup->role_player_O]++;
            break;
        default:
            for (int i = 0; i < setup->board_size_horiz; i++)
                if (board->move_histogram[i] < setup->board_size_vert)
                    return GAME_CONTINUE;
            printf("*****************\n* Game is a tie *\n*****************\n");
            break;
    }
    if (setup->role_player_X != setup->role_player_O) {
        switch (setup->role_player_X) {
            case ROLE_GUEST:
                printf("Guest AI has won %d time%s. \n", counter_win[ROLE_GUEST], (counter_win[ROLE_GUEST] == 1) ? "" : "s");
                break;
            case ROLE_HOST:
                printf("Host AI has won %d time%s. \n", counter_win[ROLE_HOST], (counter_win[ROLE_HOST] == 1) ? "" : "s");
                break;
            case ROLE_HUMAN:
                printf("Player has won %d time%s. \n", counter_win[ROLE_HUMAN], (counter_win[ROLE_HUMAN] == 1) ? "" : "s");
                break;
            default:
                exit(ERROR_ROLE);
        }
        switch (setup->role_player_O) {
            case ROLE_GUEST:
                printf("Guest AI has won %d time%s. \n", counter_win[ROLE_GUEST], (counter_win[ROLE_GUEST] == 1) ? "" : "s");
                break;
            case ROLE_HOST:
                printf("Host AI has won %d time%s. \n", counter_win[ROLE_HOST], (counter_win[ROLE_HOST] == 1) ? "" : "s");
                break;
            case ROLE_HUMAN:
                printf("Player has won %d time%s. \n", counter_win[ROLE_HUMAN], (counter_win[ROLE_HUMAN] == 1) ? "" : "s");
                break;
            default:
                exit(ERROR_ROLE);
        }
    }
    printf("\n********************************\n> Press enter to start a new game. ");
    getchar();
    return GAME_RESTART;
}

void update_board(Board *board, int last_move, GlobalSetup *setup)
{
    board->matrix[last_move][board->move_histogram[last_move]] = (board->counter_move % 2 == 0) ? FIRST_MOVE : SECOND_MOVE;
    board->last_move = last_move;
    board->move_histogram[last_move]++;
    board->move_history[board->counter_move] = last_move;
    board->counter_move++;
}

int check_winner(Board *board, GlobalSetup *setup)
{
    int last_move = board->last_move, last_move_vertical = board->move_histogram[last_move] - 1, role_player = board->matrix[last_move][last_move_vertical], counter_piece, last_move_column, last_move_row;
    
    //    check --
    counter_piece = 1;
    for (last_move_column = last_move + 1; last_move_column < setup->board_size_horiz && board->matrix[last_move_column][last_move_vertical] == role_player; last_move_column++)
        counter_piece++;
    for (last_move_column = last_move - 1; last_move_column >= 0 && board->matrix[last_move_column][last_move_vertical] == role_player; last_move_column--)
        counter_piece++;
    if (counter_piece >= setup->piece_in_row)
        return role_player;
    
    //    check |
    counter_piece = 1;
    for (last_move_row = last_move_vertical - 1; last_move_row >= 0 && board->matrix[last_move][last_move_row] == role_player; last_move_row--)
        counter_piece++;
    if (counter_piece >= setup->piece_in_row)
        return role_player;
    
    /*    check \  */
    counter_piece = 1;
    for (last_move_column = last_move - 1, last_move_row = last_move_vertical + 1; last_move_column >= 0 && last_move_row < setup->board_size_vert && board->matrix[last_move_column][last_move_row] == role_player; last_move_column--, last_move_row++)
        counter_piece++;
    for (last_move_column = last_move + 1, last_move_row = last_move_vertical - 1; last_move_column < setup->board_size_horiz && last_move_row >= 0 && board->matrix[last_move_column][last_move_row] == role_player; last_move_column++, last_move_row--)
        counter_piece++;
    if (counter_piece >= setup->piece_in_row)
        return role_player;
    
    //    check /
    counter_piece = 1;
    for (last_move_column = last_move + 1, last_move_row = last_move_vertical + 1; last_move_column < setup->board_size_horiz && last_move_row < setup->board_size_vert && board->matrix[last_move_column][last_move_row] == role_player; last_move_column++, last_move_row++)
        counter_piece++;
    for (last_move_column = last_move - 1, last_move_row = last_move_vertical - 1; last_move_column >= 0 && last_move_row >= 0 && board->matrix[last_move_column][last_move_row] == role_player; last_move_column--, last_move_row--)
        counter_piece++;
    if (counter_piece >= setup->piece_in_row)
        return role_player;
    
    return GAME_CONTINUE;
}

/******************************************************************************
 End of file
 ******************************************************************************/

