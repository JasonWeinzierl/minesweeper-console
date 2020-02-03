/* 
 * File: difficulty.c
 * Author: Jason Weinzierl
 *
 * Created 2 February 2020, 11:45
 */

#include <stdio.h>

#include "difficulty.h"

/**
 * returns a difficulty based on given integer
 */
Difficulty
newDifficulty(int option)
{
    Difficulty d = (Difficulty){0, 0, 0};

    if (option > NUM_DIFFICULTIES)
    {
        fprintf(stderr, "Given option is incorrect.\n");
        return d;
    }

    switch (option)
    {
        case 1: // easy
            d = (Difficulty){8, 8, 10};
            return d;
        case 2: // intermediate
            d = (Difficulty){16, 16, 40};
            return d;
        case 3: // expert
            d = (Difficulty){16, 30, 99};
            return d;
        default:// should never get here
            fprintf(stderr, "Internal error. Number of difficulties expected does not match implemented.\n");
            return d;
    }
}

/**
 * 1 if in bounds of difficulty's board
 * 0 if out of bounds
 */
int
isInBounds(int row, int column, Difficulty d)
{
    return row >= 0 && column >= 0 && row < d.rows && column < d.columns;
}

/**
 * returns 1 on equal,
 * 0 otherwise
 */
int
equalsDifficulty(Difficulty d1, Difficulty d2)
{
   if (d1.rows != d2.rows)
       return 0;
   if (d1.columns != d2.columns)
       return 0;
   if (d1.num_mines != d2.num_mines)
       return 0;

   return 1;
}

