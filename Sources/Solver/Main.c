/** @file Main.c
 * The Sudoku Solver solving algorithm.
 * @author Adrien RICCIARDI
 * @version 0.0.1 : 23/12/2012
 * @version 0.0.2 : 12/01/2013, applied KISS principle using only a backtrack algorithm, could solve easy grids.
 * @version 0.0.3 : 13/01/2013, added optimization to avoid backtracking when only one value can fit into a cell, could solve some medium grids.
 * @version 0.0.4 : 29/05/2013, added GridShowDifferences() to ease debug.
 * @version 0.0.5 : 29/05/2013, debugged backtrack algorithm (and removed 0.0.3 optimization which was buggy) which runs good with every examples.
 * @version 1.0.0 : 30/05/2013, code cleanup.
 * @version 1.0.1 : 31/05/2013, improved speed by changing available numbers list order.
 * @version 1.0.2 : 15/06/2013, improved speed a bit by using statically allocated custom lists.
 * @version 1.0.3 : 16/06/2013, added status report displaying.
 * @version 1.1.0 : 25/08/2013, used other approach with bitmasks for rows, columns and squares to greatly improve speed (about 90 times faster).
 * @version 1.1.1 : 16/04/2014, made a little optimization on grid access.
 * @version 1.1.2 : 18/04/2014, used a stack to list the empty cells, the solving is more than 2 times faster.
 */
#include <stdio.h>
#include <stdlib.h>
#include "Configuration.h"
#include "Grid.h"

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
// How many loops were done
static unsigned int Loops_Count = 1;
// How many bad solutions were found before the good one (or before crashing...)
static unsigned int Bad_Solutions_Found_Count = 0;
// Cache grid size value (and convert it to unsigned)
static unsigned int Grid_Size;

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** The brute force backtrack algorithm.
 * @return 1 if the grid was solved or 0 if not.
 */
static int Backtrack(void)
{
	int Row, Column;
	unsigned int Bitmask_Missing_Numbers, Tested_Number;
	
	// Find the first empty cell (don't remove the stack top now as the backtrack can return soon if no available number is found) 
	if (CellsStackReadTop(&Row, &Column) == 0)
	{
		// No empty cell remain and there is no error in the grid : the solution has been found
		if (GridIsCorrectlyFilled()) return 1;
		
		// A bad grid was generated...
		#ifdef DEBUG
			printf("[Backtrack] Bad grid generated !\n");
		#endif
		return 0;
	}
	
	// Get available numbers for this cell
	Bitmask_Missing_Numbers = GridGetCellMissingNumbers(Row, Column);
	// If no number is available a bad grid has been generated... It's safe to return here as the top of the stack has not been altered
	if (Bitmask_Missing_Numbers == 0) return 0;
	
	#ifdef DEBUG
		printf("[Backtrack] Available numbers for (row %d ; column %d) : ", Row, Column);
		GridShowBitmask(Bitmask_Missing_Numbers);
	#endif
	
	// Try each available number
	for (Tested_Number = 1; Tested_Number <= Grid_Size; Tested_Number++)
	{
		// Loop until an available number is found
		if (!(Bitmask_Missing_Numbers & (1 << Tested_Number))) continue;
		
		// Try the number
		GridSetCellValue(Row, Column, Tested_Number);
		GridRemoveCellMissingNumber(Row, Column, Tested_Number);
		CellsStackRemoveTop(); // Really try to fill this cell, removing it for next simulation step
		Loops_Count++;
		
		#ifdef DEBUG
			printf("[Backtrack] Modified grid :\n");
			GridShowDifferences(GRID_COLOR_CODE_BLUE);
			putchar('\n');
		#endif

		// Simulate next state
		if (Backtrack() == 1) return 1; // Good solution found, go to tree root
		
		// Bad solution found, restore old value
		GridSetCellValue(Row, Column, GRID_EMPTY_CELL_VALUE);
		GridRestoreCellMissingNumber(Row, Column, Tested_Number);
		CellsStackPush(Row, Column); // The cell is available again
		Bad_Solutions_Found_Count++;
		
		#ifdef DEBUG
			printf("[Backtrack] Restored grid :\n");
			GridShowDifferences(GRID_COLOR_CODE_RED);
			putchar('\n');
		#endif
	}
	// All numbers were tested unsuccessfully, go back into the tree
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	// Show the title
	printf("+---------------+\n");
	printf("| Sudoku Solver |\n");
	printf("+---------------+\n\n");
	
	// Check if a file name is provided
	if (argc != 2)
	{
		printf("Error : bad parameters.\n");
		printf("Usage : %s File_Name_Sudoku_Grid\n", argv[0]);
		return -1;
	}
	
	// Try to load the grid file
	switch (GridLoadFromFile(argv[1]))
	{
		case -1:
			printf("Error : can't open file %s.\n", argv[1]);
			return -1;
			
		case -2:
			printf("Error : grid size is bad.\nThe maximum allowed size is %d.\n", CONFIGURATION_GRID_MAXIMUM_SIZE);
			return -1;
			
		case -3:
			printf("Error : bad grid file. There are not enough numbers to fill the grid.\n");
			return -1;
	}
	Grid_Size = GridGetSize();
	
	// Show file name
	printf("File : %s.\n\n", argv[1]);
	// Show grid
	printf("Grid to solve :\n");
	GridShow();
	putchar('\n');
	
	// Start solving
	if (Backtrack() == 1)
	{
		printf("Grid successfully solved in %u loops.\n", Loops_Count);
		printf("Bad solutions found before the good one : %u\n", Bad_Solutions_Found_Count);
		printf("\nSolved grid :\n");
		GridShow();
		putchar('\n');
		return 0;
	}

	// Backtracking lead to the original grid, it is not solvable
	printf("Failure : none stategie can solve this grid, sorry.\n\n");
	printf("Found grid :\n");
	GridShow();
	putchar('\n');
	return 1;
}