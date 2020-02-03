/* 
 * File: board.c
 * Author: Jason Weinzierl
 *
 * Created 2 February 2020, 11:36
 */

#include <stdlib.h>
#include <time.h>

#include "board.h"

/**
 * constructs a new board
 */
Board *
newBoard(Difficulty d)
{
    Board *b = malloc(sizeof(struct _board));
    
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

void
randomizeSquares(Board *board)
{
    // populate board with mines
    srand(time(NULL));
    int i, n;
    int r, c;   // random square
    for (i = 1, n = board->difficulty.num_mines; i <= n; i++)
    {
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
    int j;
    for (i = 0, n = board->difficulty.rows; i < n; i++)
    {
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
 * @return the amount of mines immediately adjacent to the given space
 */
int
countMines(Board *board, int row, int column)
{
    int surroundingMines = 0;
    int i, j;
    int r, c;
    for (i = -1; i <= 1; i++) // loop vertically adjacent squares relative to given square
    {
        for (j = -1; j <= 1; j++)   // loop horizontally adjacent squares
        {
            r = row + i;    // nearby row
            c = column + j; // nearby column
            if (isInBounds(r, c, board->difficulty))    // make sure we aren't out of bounds of the board
            {
                if (board->squares[r][c].isAMine == 1)  // if the nearby square is a mine
                    surroundingMines++;  // increment the surrounding mines counter
            }
        }
    }
    
    return surroundingMines;
}

int
isAMine(Board *board, int row, int column)
{
    return board->squares[row][column].isAMine == 1;
}

/**
 * changes board based on user input
 */
void
updateBoard(Board *board, int row, int column)
{
    if (board->squares[row][column].surroundingMines == 0)
        uncoverNeighbors(board, row, column);   // if the selected square has no mines for neighbors, uncover its neighbors
    else
        board->squares[row][column].isCovered = 0;   // uncover the selected square
}

/**
 * Recursively uncovers all connecting squares that aren't mines
 * like classic minesweeper behavior
 */
void
uncoverNeighbors(Board *board, int row, int column)
{
    // BASE CASES: square is out of bounds
    if (!isInBounds(row, column, board->difficulty))
        return;
    // or square is already uncovered
    else if (board->squares[row][column].isCovered == 0)
        return;
    // or square is a mine
    else if (board->squares[row][column].isAMine == 1)
        return;
    
    // uncover the current square
    board->squares[row][column].isCovered = 0;

    // uncover nearby neighbors
    // but only if the current square has no nearby mines
    if (board->squares[row][column].surroundingMines == 0)
    {
        int i, j;
        int r, c;
        for (i = -1; i <= 1; i++)   // relative looping from previous (-1) to next (1)
        {
            for (j = -1; j <= 1; j++)
            {
                r = row + i;
                c = column + j;
                uncoverNeighbors(board, r, c);  // recursive call
            }
        }
    }
}

/**
 * returns 1 if the game is concluded,
 * returns 0 otherwise
 */
int
isAllSquaresUncovered(Board *board)
{
    int i, j;
    for (i = 0; i < board->difficulty.rows; i++)
    {
        for (j = 0; j < board->difficulty.columns; j++)
        {
            if (board->squares[i][j].isCovered == 1
                    && board->squares[i][j].isAMine != 1)
                return 0;   // if any square exists that is covered and is not a mine, the game is not over
        }
    }
    
    return 1;   // the game is won
}

/**
 * free()s board
 */
void
freeBoard(Board *board)
{
    int i;
    for (i = 0; i < board->difficulty.rows; i++)
    {
        free(board->squares[i]);
    }
    free(board->squares);
    free(board);
}
