/*
 * File: display.c
 * Author: Jason Weinzierl
 *
 * Created 1 December 2019, 10:18
 */

#include <stdio.h>

#include "display.h"
#include "scorecard.h"

/**
 * prints the board using ANSI escape sequences
 * status: 1 for default, 2 for fail, 3 for victory
 */
void
printBoard(Board *b, int status)
{
    printColumnHeader(b);
    int i, j;
    for (i = 0; i < b->difficulty.rows; i++)
    {
        printf("\x1B[0m\n%2d|", i);
        for (j = 0; j < b->difficulty.columns; j++)
        {
            if ((status != 1 && b->squares[i][j].isAMine == 1) || (status == 1 && b->squares[i][j].isCovered == 1))
            {
                switch (status)
                {
                    case 1:
                        printSquare('x', 1);
                        break;
                    case 2:
                        printSquare('M', 4);
                        break;
                    case 3:
                        printSquare('X', 5);
                        break;
                    default:
                        break;
                }
            }
            else if (b->squares[i][j].surroundingMines == 0) 
            {
                printSquare('0', 2);
            }
            else
            {
                printSquare(b->squares[i][j].surroundingMines + '0', 3);
            }
        }
    }
    printf("\x1B[0m");  // turn off attributes
}

void
printSquare(char contents, int type)
{
    switch (type)
    {
        // foreground green (zero)
        case 2:
            printf("\x1B[0;32m");
            break;
        // foreground yellow (nonzero number)
        case 3:
            printf("\x1B[0;33m");
            break;
        // foreground red bold (exploded mine)
        case 4:
            printf("\x1B[1;31m");
            break;
        // foreground cyan bold (victory mine)
        case 5:
            printf("\x1B[1;36m");
            break;
        // regular
        case 1:
        default:
            printf("\x1B[0m");
            break;
    }
    printf("%c ", contents);
    printf("\x1B[0m");  // reset
}

/*
 * prints default board
 */
void
printDefaultBoard(Board *b)
{
    printBoard(b, 1);
}

/**
 * game over display
 */
void
printUncoveredBoard(Board *b)
{
    printBoard(b, 2);
}

/**
 * reveals mines to winner
 */
void
printVictoryBoard(Board *b)
{
    printBoard(b, 3);
}

/**
 * prints header of board with varying colors for column numbers
 */
void
printColumnHeader(Board *b)
{
    int i;
    printf("\x1B[0m\n\n  _");
    for (i = 0; i < b->difficulty.columns; i++)
    {
        printf("\x1B[01;3%dm", (i % 2) ? 7 : 0);
        printf("%d", i);
        if (i < 10)
            printf("\x1B[0m_");
    }
}


void
printScores(void)
{
    // open highscore file in reading mode
    FILE *fp = fopen("scores.bin", "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening scores.bin\n");
        return;
    }

    printf("___HIGH_SCORES___\n");

    int i, n;
    fread(&n, sizeof n, 1, fp);
    printf("_Difficulties:_%d_\n", n);
    for (i = 0; i < n; i++)
    {
        struct _scorecard s;
        fread(&s, sizeof(struct _scorecard), 1, fp);
        printf("Level %d: TIME: %.2f\n\tMoves: %d\n\tBoard: %dx%d\n\tMines: %d\n", i + 1, s.time, s.moves, s.difficulty.rows, s.difficulty.columns, s.difficulty.num_mines);
    }

    if (fclose(fp) == EOF)
        fprintf(stderr, "Error closing scores.bin\n");
    return;
}
