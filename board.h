/* 
 * File: board.h
 * Author: Jason Weinzierl
 *
 * Created 2 February 2020, 11:25
 */

#ifndef BOARD_H
#define BOARD_H

#include "square.h"
#include "difficulty.h"

typedef struct _board
{
    Square **squares;
    Difficulty difficulty;
    int moves;
} Board;

void randomizeSquares(Board *);
Board *newBoard(Difficulty);
int countMines(Board *, int, int);
int isAMine(Board *, int, int);
void updateBoard(Board *, int, int);
void uncoverNeighbors(Board *, int, int);
int isAllSquaresUncovered(Board *);
void freeBoard(Board *);

#endif /* BOARD_H */
