/*
 * File: main.h
 * Author: Jason Weinzierl
 *
 * Created 1 December 2019, 09:52
 */

#ifndef MAIN_H
#define MAIN_H

#include "game.h"

int isValidArgs(int argc, char *argv[]);
Difficulty newDifficulty(int);
int getInt(int *);
void randomizeSquares(Board);
Board newBoard(Difficulty);
int countMines(Board, int, int);
int isInBounds(int, int, Difficulty);
int updateBoard(Board, int, int);
void uncoverNeighbors(Board, int, int);
int checkGameState(Board);
void freeBoard(Board);
int getHighScore(Scorecard);
int setHighScore(Scorecard);
int equalsDifficulty(Difficulty, Difficulty);
int createScoreFile(void);

#endif
