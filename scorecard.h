/* 
 * File: scorecard.h
 * Author: Jason Weinzierl
 *
 * Created 2 February 2020, 11:32
 */

#ifndef SCORECARD_H
#define SCORECARD_H

#include "difficulty.h"

typedef struct _scorecard
{
    int moves;
    double time;
    Difficulty difficulty;
} Scorecard;

int getHighScore(Scorecard);
int setHighScore(Scorecard);
int createScoreFile(void);

#endif /* SCORECARD_H */
