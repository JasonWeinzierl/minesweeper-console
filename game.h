/* 
 * File: game.h
 * Author: Jason Weinzierl
 *
 * Created 1 December 2019, 09:50
 */

#ifndef GAME_H
#define GAME_H

#define NUM_DIFFICULTIES 3

// this is a pass-by-value struct for the board difficulty
typedef struct _difficulty
{
    int rows;
    int columns;
    int num_mines;
} Difficulty;

typedef struct _square
{
    int surroundingMines;
    int isCovered;
    int isAMine;
} Square;

typedef struct _board
{
    Square **squares;
    Difficulty difficulty;
    int moves;
} *Board;

typedef struct _scorecard
{
    int moves;
    double time;
    Difficulty difficulty;
} *Scorecard;

#endif
