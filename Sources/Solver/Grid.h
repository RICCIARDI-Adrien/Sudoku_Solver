/** @file Grid.h
 * Contains all necessary functions to deal with the grid.
 * @author Adrien RICCIARDI
 * @version 1.0 : 30/05/2013
 * @version 1.1 : 31/05/2013, the list returned by GridGetCellMissingNumbers() is now in the right order as it seems to save a lot of cycles in the backtrack algorithm.
 * @version 1.2 : 16/06/2013, added GridGetEmptyCellsCount().
 * @version 1.3 : 25/08/2013, used bitmasks of all rows, columns and squares instead of cell lists.
 * @version 1.4 : 16/04/2014, made a prettier and faster code to access grid cells.
 */
#ifndef H_GRID_H
#define H_GRID_H

/** Blue terminal text color. */
#define GRID_COLOR_CODE_BLUE 0
/** Red terminal text color. */
#define GRID_COLOR_CODE_RED 1

/** Value of an empty cell. */
#define GRID_EMPTY_CELL_VALUE 0

/** Print the grid to the screen. */
void GridShow(void);

/** Print the grid and highlight differences between current grid and the one from the last function call.
 * @param Color_Code Color used to display different cells (use GRID_COLOR_CODE_XXX).
 */
void GridShowDifferences(int Color_Code);

/** Retrieve the allowed numbers for a cell.
 * @param Cell_Row Row coordinate.
 * @param Cell_Column Column coordinate.
 * @return A bitmask containing all allowed numbers.
 */
unsigned int GridGetCellMissingNumbers(int Cell_Row, int Cell_Column);

/** Tell if the grid is correctly filled (there is no bad at wrong position).
 * @return 1 if the grid is correctly filled or 0 if not.
 */
int GridIsCorrectlyFilled(void);

/** Retrieve the coordinates of the first empty cell.
 * @param Pointer_Row Row coordinate of the first empty cell.
 * @param Pointer_Column Column coordinate of the first empty cell.
 * @return 1 if an empty cell was found or 0 if all cells are filled (in this case the values of Pointer_Row and Pointer_Column are undefined).
 */
int GridGetFirstEmptyCell(int *Pointer_Row, int *Pointer_Column);

/** Set the value of a cell.
 * @param Cell_Row Row coordinate of the cell.
 * @param Cell_Column Column coordinate of the cell.
 * @param Cell_Value Value of the cell.
 * @warning No check is done on the provided coordinates, be sure to not cross grid bounds.
 */
void GridSetCellValue(int Cell_Row, int Cell_Column, int Cell_Value);

/** Load the grid content from a file.
 * @param String_File_Name Name of the file describing the grid.
 * @return 0 if the grid was correctly loaded,
 * @return -1 if the file was not found,
 * @return -2 if the grid size is too big,
 * @return -3 if the cells data are bad.
 */
int GridLoadFromFile(char *String_File_Name);

/** Find the number of empty cells into the current grid.
 * @return The number of emtpy cells.
 */
int GridGetEmptyCellsCount(void);

/** Remove a number from the cell missing ones lists.
 * @param Cell_Row Row coordinate of the cell.
 * @param Cell_Column Column coordinate of the cell.
 * @param Number The number to remove from the cell missing numbers.
 */
void GridRemoveCellMissingNumber(int Cell_Row, int Cell_Column, int Number);

/** Restore a number into the cell missing ones lists.
 * @param Cell_Row Row coordinate of the cell.
 * @param Cell_Column Column coordinate of the cell.
 * @param Number The number to restore to the cell missing numbers.
 */
void GridRestoreCellMissingNumber(int Cell_Row, int Cell_Column, int Number);

/** Show a bitmask in binary representation.
 * @param Bitmask The bitmask to visualize.
 */
void GridShowBitmask(unsigned int Bitmask);

/** Get the grid size in cells.
 * @param The grid size.
 */
int GridGetSize(void);

#endif