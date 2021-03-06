# Sudoku Solver

## Description
One more Sudoku solver program, but this one is really stupid and uses the simplest backtracking algorithm (even if it discards crazy solutions like having three '1' in the same row).

## Building the program
There are two available configurations :
* Type "make" to compile the program in release mode, which is the fastest solving mode and does not display debug information.
* Type "make debug" to compile the program in debug mode. A lot of debug information will be displayed and the program will be really slow.

## Testing the program
Open a terminal, go to the Tests directory and type "./Tests.bash". Wait some (long ?) time according to your CPU speed and hope for all the tests to succeed.  
  
Have you got a very powerful computer (one core with a very high frequency, multiple cores do not count as the program is monothreaded) ?  
Try to solve the .impossible grid and in the same time heat your house for a very long time !

## Grid files format
A grid file is a simple text file as following :
* Each cell number is represented by a single character.
* The accepted characters are '0' to 'F' (hexadecimal numbers).
* There is no space between characters.
* The length of the lines represents the grid size.
* An empty cell is represented by the point character '.'.
* The accepted grid sizes are 6x6, 9x9, 12x12 and 16x16.

Here is an example 6x6 grid :
```
2.1523
.3514.
014.52
3524.1
.03.15
5210.4
```
