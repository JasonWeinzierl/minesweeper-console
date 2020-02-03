/* 
 * File:   main.c
 * Author: Jason Weinzierl
 *
 * Created 6 October 2015, 11:58
 *
 * High scores are saved "scores.bin".  It must
 * be deleted if the preset difficulties are modified.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"
#include "display.h"
#include "board.h"
#include "difficulty.h"
#include "scorecard.h"

/**
 * returns 2 on program error,
 * returns 1 on user error,
 * returns 0 on success
 */
int
main(int argc, char *argv[])
{
    // make sure command line arguments are valid
    if (!isValidArgs(argc, argv))
        return 1;

    // user-set values
    Difficulty d;
    if (argc == 4)
    {
        d.rows = atoi(argv[1]);
        d.columns = atoi(argv[2]);
        d.num_mines = atoi(argv[3]);
    }
    // preset values
    else if (argc == 2)
    {
        d = newDifficulty(atoi(argv[1]));

        // check newDifficulty didn't fail
        Difficulty error = (Difficulty){0, 0, 0};
        if (equalsDifficulty(d, error))
            return 2;
    }
    // default difficulty
    else
    {
        d = newDifficulty(1);
    }

    // construct board, randomize squares
    Board *board = newBoard(d);

    // start timer
    time_t start, end;
    double diff;
    time(&start);

    // main loop
    while (1)
    {
        printDefaultBoard(board);
        
        // get row
        printf("\n\nEnter the row of the space you want to hit: ");
        int row = -1;
        while (!getInt(&row) || row >= board->difficulty.rows || row < 0) // safely get int, and error check
        {
            // CHEATER
            if (row == -42)
            {
                diff = 69420.80085; // no high score for cheaters
                goto VICTORY;   // goto considered harmful
            }
            printf("Incorrect selection. Enter row: ");
        }

        // get column
        printf("\nEnter the column of the space you want to hit: ");
        int column = -1;
        while (!getInt(&column) || column >= board->difficulty.columns || column < 0)
        {
            printf("Incorrect selection. Enter column: ");
        }
        
        // hit desired space, check if u lose
        board->moves++;
        if (isAMine(board, row, column))
        {
            // stop timer
            time(&end);
            diff = difftime(end, start);

            printUncoveredBoard(board);
            printf("\nYou hit a mine! %d move%s and %.1f seconds. Game Over\n", board->moves, (board->moves == 1) ? "" : "s", diff);
            freeBoard(board);
            return 0; // end of while-loop
        }
        else
        {
            updateBoard(board, row, column);
        }
        
        if (isAllSquaresUncovered(board))
        {
            // stop timer
            time(&end);
            diff = difftime(end, start);

            VICTORY:

            printVictoryBoard(board);
            printf("\nYou won in %d move%s and %.1f seconds! Congratulations!\n", board->moves, (board->moves == 1) ? "" : "s", diff);
            break;  // end of while-loop
        }
    }

    // create scorecard from board data
    Scorecard *s = malloc(sizeof(struct _scorecard));
    s->moves = board->moves;
    s->time = diff;
    s->difficulty = board->difficulty;

    freeBoard(board);

    // determine if score is high score
    switch (getHighScore(*s))
    {
        case -1:
            fprintf(stderr, "Error opening score file.\n");
            break;
        case 1:
            printf("\x1B[01;31mNew High Score!\x1B[0m\n");
            setHighScore(*s);
            
            printScores();
            break;
        case 2:
            printf("Custom Difficulties are not supported for high scores.\n");
            /* FALLTHROUGH */
        default:
            printScores();
            printf("Thank you for playing!\n");
    }

    free(s);

    return (EXIT_SUCCESS);
}

/**
 * returns 1 on valid command line arguments,
 * returns 0 otherwise
 */
int
isValidArgs(int argc, char *argv[])
{
    // no args is ok
    if (argc == 1)
        return 1;

    // preset difficulty selector
    if (argc == 2)
    {
        int option = atoi(argv[1]);
        
        // difficulty does not exist
        if (option <= 0 || option > NUM_DIFFICULTIES)
        {
            fprintf(stderr, "Usage: ./minesweeper.out [difficulty=1,2,3]\n");
            return 0;
        }

        return 1;
    }

    // custom difficulty

    // incorrect number of args
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./minesweeper.out [rows] [columns] [num_mines]\n");
        return 0;
    }

    // incorrect first arg
    int rows = atoi(argv[1]);
    if (rows <= 0)
    {
        fprintf(stderr, "\"%s\" is not a valid row size\n", argv[1]);
        return 0;
    }
    // incorrect second arg
    int columns = atoi(argv[2]);
    if (columns <= 0)
    {
        fprintf(stderr, "\"%s\" is not a valid column size\n", argv[2]);
        return 0;
    }
    // incorrect dimensions (overflow or too big)
    if (rows * columns <= 0 || rows * columns >= 10000)
    {
        fprintf(stderr, "Dimensions %dx%d are not valid\n", rows, columns);
        return 0;
    }
    // incorrect third arg
    int num_mines = atoi(argv[3]);
    if (num_mines <= 0)
    {
        fprintf(stderr, "\"%s\" is not a valid amount of mines\n", argv[3]);
        return 0;
    }
    // too many mines
    if (num_mines > rows * columns)
    {
        fprintf(stderr, "Amount of mines is not valid for given dimensions %dx%d\n", rows, columns);
        return 0;
    }

    return 1;

}

/**
 * get input without breaking on non-int input
 * 
 * returns 1 if successful input,
 * returns 0 if invalid input
 *
 * NOTE: successful input does not clear input stream
 */
int
getInt(int *input)
{
    if (scanf("%d", input) <= 0)
    {
        // when scanf() fails
        // (ie entering a non-number),
        // flush remaining characters out of input stream:
        char c = 0;
        while ( (c = getchar()) != '\n' && c != EOF );   // get chars until we reach the end.
        return 0;   // failed
    }
    
    return 1;       // succeeded
}

