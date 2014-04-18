/** @file Cells_Stack.h
 * A very simple implementation of an unique stack gathering all empty cells.
 * @author Adrien RICCIARDI
 * @version 1.0 : 17/04/2014
 */
#ifndef H_CELLS_STACK_H
#define H_CELLS_STACK_H

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Set the stack as empty. */
void CellsStackInitialize(void);

/** Push a cell coordinates on the top of the stack.
 * @param Cell_Row The cell row coordinate.
 * @param Cell_Column The cell column coordinate.
 * @return 1 if the cell coordinates were correctly pushed,
 * @return 0 if the stack is full.
 */
int CellsStackPush(int Cell_Row, int Cell_Column);

/** Get the top of the stack without removing it.
 * @param Pointer_Cell_Row On output, will contain the top of stack cell's row coordinate.
 * @param Pointer_Cell_Column On output, will contain the top of stack cell's column coordinate.
 * @return 1 if the cell was correctly poped,
 * @return 0 if the stack is empty.
 */
int CellsStackReadTop(int *Pointer_Cell_Row, int *Pointer_Cell_Column);

/** Remove the top of the stack. Do nothing if the stack is empty. */
void CellsStackRemoveTop(void);

#endif