/*
 * File: display.h
 * Author: Jason Weinzierl
 *
 * Created 1 December 2019, 10:19
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "board.h"

void printBoard(Board *, int);
void printSquare(char, int);
void printDefaultBoard(Board *);
void printUncoveredBoard(Board *);
void printVictoryBoard(Board *);
void printColumnHeader(Board *);
void printScores(void);

#endif /* DISPLAY_H */
