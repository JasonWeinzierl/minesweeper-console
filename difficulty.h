/* 
 * File: difficulty.h
 * Author: Jason Weinzierl
 *
 * Created 2 February 2020, 11:26
 */

#ifndef DIFFICULTY_H
#define DIFFICULTY_H

#define NUM_DIFFICULTIES 3

typedef struct _difficulty
{
    int rows;
    int columns;
    int num_mines;
} Difficulty;

/**
 * returns a difficulty based on given integer
 */
Difficulty newDifficulty(int);

/**
 * 1 if in bounds of difficulty's board
 * 0 if out of bounds
 */
int isInBounds(int, int, Difficulty);

/**
 * returns 1 on equal,
 * 0 otherwise
 */
int equalsDifficulty(Difficulty, Difficulty);

#endif /* DIFFICULTY_H */
