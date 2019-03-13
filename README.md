# minesweeper-console
A simple minesweeper that runs in the console, written in C.

Initially created in October 2015 for CS 2050.

## Features
- uses recursion to open empty spots
- saves high scores to `scores.bin`
- cheat mode, which disables high score
- uses a goto statement
- this would be easier with OOP…

## Usage
`make`

Play on **Easy**: `./minesweeper.out`

Choose between **Easy**, **Intermediate**, and **Expert**: `./minesweeper.out [difficulty=1,2,3]`

**Custom** difficulty (scores not saved): `./minesweeper.out [rows] [columns] [num_mines]`

`scores.bin` will be created after the first game ends, if it doesn't already exist.
