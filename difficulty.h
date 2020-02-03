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

Difficulty newDifficulty(int);
int isInBounds(int, int, Difficulty);
int equalsDifficulty(Difficulty, Difficulty);

#endif /* DIFFICULTY_H */
