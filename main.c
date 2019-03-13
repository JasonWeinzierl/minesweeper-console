/* 
 * File:   main.c
 * Author: Jason Weinzierl
 *
 * Initially created on October 6, 2015, 11:58 AM
 *
 * High scores are saved "scores.bin".  It must
 * be deleted if the preset difficulties are modified.
 *
 * This would be neater in an OOP language.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>       // timer functions
#include <limits.h>     // INT_MAX
#include <float.h>      // DBL_MAX

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
    int rows;
    Difficulty difficulty;
    int moves;
} *Board;

typedef struct _scorecard
{
    int moves;
    double time;
    Difficulty difficulty;
} *Scorecard;

int validateArgs(int argc, char *argv[]);
Difficulty newDifficulty(int);
int getInt(int *);
void randomizeSquares(Board);
Board newBoard(Difficulty);
int countMines(Board, int, int);
void printBoard(Board);
void printUncoveredBoard(Board);
int updateBoard(Board, int, int);
void uncoverNeighbors(Board, int, int);
int checkGameState(Board);
void printVictoryBoard(Board);
void printColumnHeader(Board);
void freeBoard(Board);
int getHighScore(Scorecard);
int setHighScore(Scorecard);
int equalsDifficulty(Difficulty, Difficulty);
void printScores(void);
int createScoreFile(void);

/**
 * returns 2 on program error,
 * returns 1 on user error,
 * returns 0 on success
 */
int
main(int argc, char *argv[])
{
    // make sure command line arguments are valid
    if (!validateArgs(argc, argv))
        return 1;

    // user-set values
    Difficulty d;
    if (argc == 4)
    {
        d.rows = atoi(argv[1]);
        d.columns = atoi(argv[2]);
        d.num_mines = atoi(argv[3]);
    }
    // preset values
    else if (argc == 2)
    {
        d = newDifficulty(atoi(argv[1]));

        // check newDifficulty didn't fail
        Difficulty error = (Difficulty){0, 0, 0};
        if (equalsDifficulty(d, error))
            return 2;
    }
    // default difficulty
    else
    {
        d = newDifficulty(1);
    }

    // construct board, randomize squares
    Board board = newBoard(d);

    // start timer
    time_t start, end;
    double diff;
    time(&start);

    // main loop
    while(1)
    {
        printBoard(board);
        
        // get row
        printf("\n\nEnter the row of the space you want to hit: ");
        int row = -1;
        while (!getInt(&row) || row >= board->difficulty.rows || row < 0) // safely get int, and error check
        {
            // CHEATER CHEATER PUMPKIN EATER
            if (row == -42)
            {
                diff = 69420.80085; // NO HIGH SCORE FOR YOU
                goto VICTORY;   // YES I USED A GOTO STATEMENT
            }
            printf("Incorrect selection. Enter row: ");
        }

        // get column
        printf("\nEnter the column of the space you want to hit: ");
        int column = -1;
        while (!getInt(&column) || column >= board->difficulty.columns || column < 0)
        {
            printf("Incorrect selection. Enter column: ");
        }
        
        // hit desired space, check if u lose
        board->moves++;
        if(updateBoard(board, row, column))
        {
            // stop timer
            time(&end);
            diff = difftime(end, start);

            printUncoveredBoard(board);
            printf("\nYou hit a mine! %d move%s and %.1f seconds. Game Over\n", board->moves, (board->moves == 1) ? "" : "s", diff);
            freeBoard(board);
            return 0; // end of while-loop
        }
        
        // check if board is solved
        if(checkGameState(board))
        {
            // stop timer
            time(&end);
            diff = difftime(end, start);

            VICTORY:

            printVictoryBoard(board);
            printf("\nYou won in %d move%s and %.1f seconds! Congratulations!\n", board->moves, (board->moves == 1) ? "" : "s", diff);
            break;  // end of while-loop
        }
    }

    // create scorecard from board data
    Scorecard s = malloc(sizeof(struct _scorecard));
    s->moves = board->moves;
    s->time = diff;
    s->difficulty = board->difficulty;

    freeBoard(board);

    // determine if score is high score
    switch (getHighScore(s))
    {
        case -1:
            fprintf(stderr, "Error opening score file.\n");
            break;
        case 1:
            printf("\x1B[01;31mNew High Score!\x1B[0m\n");
            setHighScore(s);
            
            printScores();
            break;
        case 2:
            printf("Custom Difficulties are not supported for high scores.\n");
            /* FALLTHROUGH */
        default:
            
            printScores();
            printf("Thank you for playing!\n");
    }

    free(s);

    return (EXIT_SUCCESS);
}

/**
 * returns 1 on valid command line arguments,
 * returns 0 otherwise
 */
int
validateArgs(int argc, char *argv[])
{
    // no args
    if (argc == 1)
        return 1;

    // preset difficulty selector
    if (argc == 2)
    {
        int option = atoi(argv[1]);
        
        // difficulty does not exist
        if (option <= 0 || option > 3)
        {
            fprintf(stderr, "Usage: ./minesweeper.out [difficulty=1,2,3]\n");
            return 0;
        }

        return 1;
    }

    // custom difficulty

    // incorrect number of args
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./minesweeper.out [rows] [columns] [num_mines]\n");
        return 0;
    }

    // incorrect first arg
    int rows = atoi(argv[1]);
    if (rows <= 0)
    {
        fprintf(stderr, "\"%s\" is not a valid row size\n", argv[1]);
        return 0;
    }
    // incorrect second arg
    int columns = atoi(argv[2]);
    if (columns <= 0)
    {
        fprintf(stderr, "\"%s\" is not a valid column size\n", argv[2]);
        return 0;
    }
    // incorrect dimensions (overflow or too big)
    if (rows * columns <= 0 || rows * columns >= 10000)
    {
        fprintf(stderr, "Dimensions %dx%d are not valid\n", rows, columns);
        return 0;
    }
    // incorrect third arg
    int num_mines = atoi(argv[3]);
    if (num_mines <= 0)
    {
        fprintf(stderr, "\"%s\" is not a valid amount of mines\n", argv[3]);
        return 0;
    }
    // too many mines
    if (num_mines >= rows * columns)
    {
        fprintf(stderr, "Amount of mines is not valid for given dimensions\n");
        return 0;
    }

    return 1;

}

/**
 * returns a difficulty based on given integer
 */
Difficulty
newDifficulty(int option)
{
    Difficulty d = (Difficulty){0, 0, 0};
    switch(option)
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
        default:// error
            fprintf(stderr, "Given option is incorrect.\n");
            return d;
    }
}

/**
 * get input without breaking on non-int input
 * 
 * returns 1 if successful input,
 * returns 0 if invalid input
 *
 * NOTE: successful input does not clear input stream
 */
int
getInt(int *input)
{
    if (scanf("%d", input) <= 0)
    {
        // when scanf() fails
        // (ie entering a non-number),
        // flush remaining characters out of input stream:
        char c = 0;
        while ( (c = getchar()) != '\n' && c != EOF );   // get chars until we reach the end.
        return 0;   // failed
    }
    
    return 1;       // succeeded
}

void
randomizeSquares(Board board)
{
    // populate board with mines
    srand(time(NULL));
    int i;
    for (i = 0; i < board->difficulty.num_mines; i++)
    {
        int r, c;   // random square
        do
        {
            // get random square
            r = rand() % board->difficulty.rows;
            c = rand() % board->difficulty.columns;
        }
        while (board->squares[r][c].isAMine == 1);   // make sure we didn't get a square that's already a mine
        
        // set the square as a mine
        board->squares[r][c].isAMine = 1;
    }
    
    // fill in remaining squares
    for (i = 0; i < board->difficulty.rows; i++)
    {
        int j;
        for (j = 0; j < board->difficulty.columns; j++)
        {
            // if current square is NOT a mine
            if (board->squares[i][j].isAMine != 1)
            {
                // count the surrounding mines
                board->squares[i][j].surroundingMines = countMines(board, i, j);
            }
            else
            {
                board->squares[i][j].surroundingMines = 0;
            }
            
            // cover the current square
            board->squares[i][j].isCovered = 1;
        }
    }
}

/**
 * constructs a new board
 */
Board
newBoard(Difficulty d)
{
    Board b = malloc(sizeof(struct _board));
    
    b->squares = malloc(sizeof(Square *) * d.rows);
    int i;
    for (i = 0; i < d.rows; i++)
        b->squares[i] = malloc(sizeof(Square) * d.columns);

    b->difficulty = d;
    b->moves = 0;

    // initialize squares
    randomizeSquares(b);

    return b;
}

/**
 * @return the amount of mines immediately adjacent to the given space
 */
int
countMines(Board board, int row, int column)
{
    int surroundingMines = 0;
    int i;
    for (i = -1; i <= 1; i++) // loop vertically adjacent squares relative to given square
    {
        int j;
        for (j = -1; j <= 1; j++)   // loop horizontally adjacent squares
        {
            int r = row + i;    // nearby row
            int c = column + j; // nearby column
            if (r >= 0 && c >= 0 && r < board->difficulty.rows && c < board->difficulty.columns)  // make sure we aren't out of bounds of the board
            {
                if (board->squares[r][c].isAMine == 1)               // if the nearby square is a mine
                    surroundingMines++;  // increment the surrounding mines counter
            }
        }
    }
    
    return surroundingMines;
}

/**
 * prints the board using ANSI escape sequences
 */
void
printBoard(Board b)
{
    printColumnHeader(b);
    int i, j;
    for (i = 0; i < b->difficulty.rows; i++)
    {
        printf("\x1B[0m\n%2d|", i);
        for (j = 0; j < b->difficulty.columns; j++)
        {
            if (b->squares[i][j].isCovered == 1)
            {
                printf("\x1B[0mx ");
            }
            else if (b->squares[i][j].surroundingMines == 0) 
            {
                printf("\x1B[32m0 ");
            }
            else
            {
                printf("\x1B[33m%d ", b->squares[i][j].surroundingMines);
            }
        }
    }
    printf("\x1B[0m");  // turn off attributes
}

/**
 * game over display
 */
void
printUncoveredBoard(Board b)
{
    printColumnHeader(b);
    int i, j;
    for(i = 0; i < b->difficulty.rows; i++)
    {
        printf("\x1B[0m\n%2d|", i);
        for(j = 0; j < b->difficulty.columns; j++)
        {
            if(b->squares[i][j].isAMine)
            {
                printf("\x1B[01;31mM ");
            }
            else if (b->squares[i][j].surroundingMines == 0) 
            {
                printf("\x1B[00;32m0 ");
            }
            else
            {
                printf("\x1B[00;33m%d ", b->squares[i][j].surroundingMines);
            }
        }
    }
    printf("\x1B[0m");
}

/**
 * reveals mines to winner
 */
void
printVictoryBoard(Board b)
{
    printColumnHeader(b);
    int i, j;
    for(i = 0; i < b->difficulty.rows; i++)
    {
        printf("\x1B[0m\n%2d|", i);
        for(j = 0; j < b->difficulty.columns; j++)
        {
            if(b->squares[i][j].isAMine)
            {
                printf("\x1B[01;36mX ");
            }
            else if (b->squares[i][j].surroundingMines == 0) 
            {
                printf("\x1B[00;32m0 ");
            }
            else
            {
                printf("\x1B[00;33m%d ", b->squares[i][j].surroundingMines);
            }
        }
    }
    printf("\x1B[0m");
}

/**
 * prints header of board with varying colors for column numbers
 */
void
printColumnHeader(Board b)
{
    int i;
    printf("\x1B[0m\n\n  _");
    for (i = 0; i < b->difficulty.columns; i++)
    {
        printf("\x1B[01;3%dm", (i % 2) ? 7 : 0);
        printf("%d", i);
        if (i < 10)
            printf("\x1B[0m_");
    }
}

/**
 * changes board based on user input
 */
int
updateBoard(Board board, int row, int column)
{
    if (board->squares[row][column].isAMine == 1)
        return 1;   // if the selected square is a mine, u lose
    else if (board->squares[row][column].surroundingMines == 0)
        uncoverNeighbors(board, row, column);   // if the selected square has no mines for neighbors, uncover its neighbors
    else
        board->squares[row][column].isCovered = 0;   // uncover the selected square
    
    return 0;
}

/**
 * The recursive implementation uncovers all connecting squares that aren't mines,
 * including squares that aren't direct neighbors (like classic minesweeper behavior).
 */
void
uncoverNeighbors(Board board, int row, int column)
{
    // BASE CASE: square is out of bounds OR square is already uncovered
    if (row < 0 || column < 0 || row >= board->difficulty.rows || column >= board->difficulty.columns)
        return;
    else if (board->squares[row][column].isCovered == 0)
        return;
    
    if (board->squares[row][column].isAMine != 1)    // if the current square is not a mine
    {
        board->squares[row][column].isCovered = 0;   // uncover the current square
    
        // recursive call on all nearby neighbors
        if (board->squares[row][column].surroundingMines == 0)   // but only if the current square has no nearby mines
        {
            int i;
            for (i = -1; i <= 1; i++)   // relative looping from previous (-1) to next (1)
            {
                int j;
                for (j = -1; j <= 1; j++)
                {
                    int r = row + i;
                    int c = column + j;
                    uncoverNeighbors(board, r, c);  // recursive call here
                }
            }
        }
    }
}

/**
 * returns 1 if the game is concluded,
 * returns 0 otherwise
 */
int
checkGameState(Board board)
{
    int i;
    for (i = 0; i < board->difficulty.rows; i++)
    {
        int j;
        for (j = 0; j < board->difficulty.columns; j++)
        {
            if (board->squares[i][j].isCovered == 1 && board->squares[i][j].isAMine != 1)
                return 0;   // if any square exists that is covered and is not a mine, the game is not over
        }
    }
    
    return 1;   // the game is won
}

/**
 * free()s board
 */
void
freeBoard(Board board)
{
    int i;
    for (i = 0; i < board->difficulty.rows; i++)
    {
        free(board->squares[i]);
    }
    free(board->squares);
    free(board);
}

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
        if (equalsDifficulty(scorecard->difficulty, d[i]))
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

        if (equalsDifficulty(scorecard->difficulty, s.difficulty))
        {
            if (scorecard->time < s.time)
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
        if (equalsDifficulty(scorecard->difficulty, s.difficulty))
        {
            fseek(fp, -sizeof(struct _scorecard), SEEK_CUR);    // go back
            fwrite(scorecard, sizeof(struct _scorecard), 1, fp);   // overwrite scorecard
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

void
printScores(void)
{
    // open or create highscore file
    FILE *fp = fopen("scores.bin", "rb");
    if (fp == NULL)
    {
        if (!createScoreFile())
        {
            fprintf(stderr, "Error opening scores.bin\n");
            return;
        }

        // open in reading mode
        fp = fopen("scores.bin", "rb");
        if (fp == NULL)
        {
            fprintf(stderr, "Error opening scores.bin\n");
            return;
        }
    }

    printf("___HIGH_SCORES___\n");

    int i, n;
    fread(&n, sizeof n, 1, fp);
    printf("_Difficulties:_%d_\n", n);
    for (i = 0; i < n; i++)
    {
        struct _scorecard s;
        fread(&s, sizeof(struct _scorecard), 1, fp);
        printf("Level %d: TIME: %.2f\n\tMoves: %d\n\tBoard: %dx%d\n\tMines: %d\n", i + 1, s.time, s.moves, s.difficulty.rows, s.difficulty.columns, s.difficulty.num_mines);
    }

    if (fclose(fp) == EOF)
        fprintf(stderr, "Error closing scores.bin\n");
    return;
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
            struct _scorecard s = (struct _scorecard){INT_MAX, DBL_MAX, d[i]};
            fwrite(&s, sizeof(struct _scorecard), 1, fp);
        }

        // close writing mode
        if (fclose(fp) == EOF)
            return 0;

        printf("Done!\n");
        return 1;
    }
}
