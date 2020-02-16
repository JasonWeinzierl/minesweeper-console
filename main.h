/*
 * File: main.h
 * Author: Jason Weinzierl
 *
 * Created 1 December 2019, 09:52
 */

#ifndef MAIN_H
#define MAIN_H

int isValidArgs(int argc, char *argv[]);
int isValidDifficultyNum(char *arg);
int isValidCustomDifficulty(char *argv[]);
int isValidNumRows(int);
int isValidNumColumns(int);
int isValidDimensions(int rows, int columns);
int isValidNumMines(int);
int getInt(int *);

#endif /* MAIN_H */
