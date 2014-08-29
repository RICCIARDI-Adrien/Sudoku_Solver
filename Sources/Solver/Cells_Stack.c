/** @file Cells_Stack.c
 * @see Cells_Stack.h for description.
 * @author Adrien RICCIARDI
 */
#include <Cells_Stack.h>
#include <Configuration.h>
#include <stdio.h>

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** How many elements the stack can hold. */
#define CELLS_STACK_SIZE (CONFIGURATION_GRID_MAXIMUM_SIZE * CONFIGURATION_GRID_MAXIMUM_SIZE)

//-------------------------------------------------------------------------------------------------
// Private types
//-------------------------------------------------------------------------------------------------
/** A cell coordinates. */
typedef struct
{
	int Row; //! The cell row coordinate.
	int Column; //! The cell column coordinate.
} TCellCoordinates;

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The stack content. */
static TCellCoordinates Stack[CELLS_STACK_SIZE];

/** The stack pointer. This is a fully ascending stack. */
static int Stack_Pointer;

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
#ifdef DEBUG
	static void CellsStackShow(void)
	{
		int i;
		
		printf("Count = %d\n", Stack_Pointer);
		for (i = Stack_Pointer - 1; i >= 0; i--) printf("Row = %d, Column = %d\n", Stack[i].Row, Stack[i].Column);
	}
#endif

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void CellsStackInitialize(void)
{
	Stack_Pointer = 0;
}

int CellsStackPush(int Cell_Row, int Cell_Column)
{
	TCellCoordinates *Pointer_Cell_Coordinates;
	
	if (Stack_Pointer >= CELLS_STACK_SIZE) return 0;
	
	// Cache coordinates address
	Pointer_Cell_Coordinates = &Stack[Stack_Pointer];
	Pointer_Cell_Coordinates->Row = Cell_Row;
	Pointer_Cell_Coordinates->Column = Cell_Column;
	Stack_Pointer++;
	
	#ifdef DEBUG
		printf("[CellsStackPush] ");
		CellsStackShow();
	#endif
	return 1;
}

int CellsStackReadTop(int *Pointer_Cell_Row, int *Pointer_Cell_Column)
{
	TCellCoordinates *Pointer_Cell_Coordinates;
	
	if (Stack_Pointer <= 0) return 0;
	
	// Cache coordinates address
	Pointer_Cell_Coordinates = &Stack[Stack_Pointer - 1];
	*Pointer_Cell_Row = Pointer_Cell_Coordinates->Row;
	*Pointer_Cell_Column = Pointer_Cell_Coordinates->Column;
	
	#ifdef DEBUG
		printf("[CellsStackReadTop] ");
		CellsStackShow();
	#endif
	return 1;
}

void CellsStackRemoveTop(void)
{
	if (Stack_Pointer > 0) Stack_Pointer--;
}

