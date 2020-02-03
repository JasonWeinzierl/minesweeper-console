/* 
 * File: scorecard.c
 * Author: Jason Weinzierl
 *
 * Created 2 February 2020, 11:41
 */

#include <stdio.h>
#include <limits.h>
#include <float.h>

#include "scorecard.h"

/**
 * returns -1 if file fails to open
 * returns 1 if score is new highscore
 * returns 2 if custom difficulty
 * returns 0 otherwise
 *
 * binary file format:
 *      [number of difficulties num_diff]
 *      [scorecard for difficulty 1]
 *      [scorecard for difficulty 2]
 *      .
 *      .
 *      .
 *      [scorecard for difficulty num_diff]
 * 
 * difficulties should be sorted by rows, then columns, the num_mines
 */
int
getHighScore(Scorecard scorecard)
{
    // get array of difficulties and compare to given
    Difficulty d[NUM_DIFFICULTIES];
    int i, found = 0;
    for (i = 0; i < NUM_DIFFICULTIES; i++)
    {
        d[i] = newDifficulty(i + 1);
        if (equalsDifficulty(scorecard.difficulty, d[i]))
            found = 1;
    }
    if (!found) // custom difficulties don't get stored
        return 2;

    // open or create highscore file
    FILE *fp = fopen("scores.bin", "rb");
    if (fp == NULL)
    {
        if (!createScoreFile())
            return -1;

        // open in reading mode
        fp = fopen("scores.bin", "rb");
        if (fp == NULL)
            return -1;
    }

    int n;
    fread(&n, sizeof n, 1, fp);

    for (i = 0; i < n; i++)
    {
        struct _scorecard s;
        fread(&s, sizeof(struct _scorecard), 1, fp);

        if (equalsDifficulty(scorecard.difficulty, s.difficulty))
        {
            if (scorecard.time < s.time)
            {
                if (fclose(fp) == EOF)
                    return -1;
                return 1;
            }
        }
    }

    if (fclose(fp) == EOF)
        return -1;

    return 0;
}

/**
 * returns -1 if file error
 * return 0 otherwise
 */
int
setHighScore(Scorecard scorecard)
{
    // open or create highscore file
    FILE *fp = fopen("scores.bin", "rb+");
    if (fp == NULL)
    {
        if (!createScoreFile())
            return -1;

        // open in reading mode
        fp = fopen("scores.bin", "rb+");
        if (fp == NULL)
            return -1;
    }

    int i, n;
    fread(&n, sizeof n, 1, fp);
    for (i = 0; i < n; i++)
    {
        struct _scorecard s;
        fread (&s, sizeof(struct _scorecard), 1, fp);
        if (equalsDifficulty(scorecard.difficulty, s.difficulty))
        {
            fseek(fp, -sizeof(struct _scorecard), SEEK_CUR);    // go back
            fwrite(&scorecard, sizeof(struct _scorecard), 1, fp);   // overwrite scorecard
            if (fclose(fp) == EOF)
                return -1;
            return 0;
        }
    }

    if (fclose(fp) == EOF)
        return -1;
    return 0;
}

/**
 * returns 1 on file created/already exists
 * returns 0 if error
 */
int
createScoreFile(void)
{
    FILE *fp;
    if ((fp = fopen("scores.bin", "rb")) != NULL)
    {
        fclose(fp);
        return 1;
    }
    else
    {
        printf("Creating new high score file... ");
        // make array of difficulties
        Difficulty d[NUM_DIFFICULTIES];
        int i;
        for (i = 0; i < NUM_DIFFICULTIES; i++)
            d[i] = newDifficulty(i + 1);

        fp = fopen("scores.bin", "wb");
        if (fp == NULL)
            return 0;

        // write number
        int n = NUM_DIFFICULTIES;
        fwrite(&n, sizeof n, 1, fp);

        // write default scorecards
        for (i = 0; i < n; i++)
        {
            Scorecard s = (struct _scorecard){INT_MAX, DBL_MAX, d[i]};
            fwrite(&s, sizeof(struct _scorecard), 1, fp);
        }

        // close writing mode
        if (fclose(fp) == EOF)
            return 0;

        printf("Done!\n");
        return 1;
    }
}

