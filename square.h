/* 
 * File: square.h
 * Author: Jason Weinzierl
 *
 * Created 2 February 2020, 11:30
 */

#ifndef SQUARE_H
#define SQUARE_H

typedef struct _square
{
    int surroundingMines;
    int isCovered;
    int isAMine;
} Square;

#endif /* SQUARE_H */
