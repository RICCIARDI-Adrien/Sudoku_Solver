/** @file Grid.c
 * @see Grid.h for description.
 * @author Adrien RICCIARDI
 */
#include <assert.h>
#include <Cells_Stack.h>
#include <Configuration.h>
#include <Grid.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------
// Private macros
//-------------------------------------------------------------------------------------------------
/** Get the square index (in the Bitmask_Squares array) in which the cell is located. The formula is Square_Row * Squares_Horizontal_Count + Square_Column.
 * @param Row Cell row coordinate.
 * @param Column Cell column coordinate.
 * @return The square index in the Bitmask_Squares array.
 */
#define GRID_GET_CELL_SQUARE_INDEX(Row, Column) ((Row / Square_Height) * Squares_Horizontal_Count + (Column / Square_Width))

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The grid. */
static int Grid[CONFIGURATION_GRID_MAXIMUM_SIZE][CONFIGURATION_GRID_MAXIMUM_SIZE];
/** Current grid side size in cells. */
static unsigned int Grid_Size;
/** Dimensions of a square in cells. */
static unsigned int Square_Width, Square_Height, Squares_Horizontal_Count, Squares_Vertical_Count;
/** The grid starting number (usually 0 or 1) added to all cell values when the grid is displayed. */
static int Grid_Display_Starting_Number;

/** All row bitmasks. */
static unsigned int Bitmask_Rows[CONFIGURATION_GRID_MAXIMUM_SIZE];
/** All column bitmasks. */
static unsigned int Bitmask_Columns[CONFIGURATION_GRID_MAXIMUM_SIZE];
/** All square bitmasks. */
static unsigned int Bitmask_Squares[CONFIGURATION_GRID_MAXIMUM_SIZE];

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Create the initial bitmasks for all rows, columns and squares. */
static inline void GridGenerateInitialBitmasks(void)
{
	unsigned int Row, Column, Number, Column_Start, Row_End, Column_End, Square_Row, Square_Column, i;
	int Is_Number_Found[CONFIGURATION_GRID_MAXIMUM_SIZE];
	
	// Parse all rows
	for (Row = 0; Row < Grid_Size; Row++)
	{
		// Initialize values
		Bitmask_Rows[Row] = 0;
		memset(Is_Number_Found, 0, sizeof(Is_Number_Found));
		
		// Find available numbers
		for (Column = 0; Column < Grid_Size; Column++)
		{
			Number = Grid[Row][Column];
			if (Number != GRID_EMPTY_CELL_VALUE) Is_Number_Found[Number] = 1;
		}
		
		// Set corresponding bits into bitmask
		for (Number = 0; Number < Grid_Size; Number++)
		{
			if (!Is_Number_Found[Number]) Bitmask_Rows[Row] |= 1 << Number;
		}
	}
	
	// Parse all columns
	for (Column = 0; Column < Grid_Size; Column++)
	{
		// Initialize values
		Bitmask_Columns[Column] = 0;
		memset(Is_Number_Found, 0, sizeof(Is_Number_Found));
		
		// Find available numbers
		for (Row = 0; Row < Grid_Size; Row++)
		{
			Number = Grid[Row][Column];
			if (Number != GRID_EMPTY_CELL_VALUE) Is_Number_Found[Number] = 1;
		}
		
		// Set corresponding bits into bitmask
		for (Number = 0; Number < Grid_Size; Number++)
		{
			if (!Is_Number_Found[Number]) Bitmask_Columns[Column] |= 1 << Number;
		}
	}
	
	// Parse all squares
	i = 0; // Index in the square bitmasks array
	
	for (Square_Row = 0; Square_Row < Squares_Vertical_Count; Square_Row++)
	{
		for (Square_Column = 0; Square_Column < Squares_Horizontal_Count; Square_Column++)
		{
			// Initialize values
			Bitmask_Squares[i] = 0;
			memset(Is_Number_Found, 0, sizeof(Is_Number_Found));
			
			// Determine coordinates of the square to parse
			Row = Square_Row * Square_Height;
			Column_Start = Square_Column * Square_Width;
			Row_End = Row + Square_Height;
			Column_End = Column_Start + Square_Width;
			
			// Find numbers present into the square
			for ( ; Row < Row_End; Row++)
			{
				for (Column = Column_Start; Column < Column_End; Column++)
				{
					Number = Grid[Row][Column];
					if (Number != GRID_EMPTY_CELL_VALUE) Is_Number_Found[Number] = 1;
				}
			}
			
			// Set corresponding bits into bitmask
			for (Number = 0; Number < Grid_Size; Number++)
			{
				if (!Is_Number_Found[Number]) Bitmask_Squares[i] |= 1 << Number;
			}
			
			// Next square bitmask
			i++;
		}
	}
}

/** Fill the stack with empty cells. */
static void GridFillStackWithEmptyCells(void)
{
	int Row, Column;
	
	CellsStackInitialize();
	
	// Reverse parsing to create a stack with empty coordinates beginning on the top-left part of the grid (this allows to make speed comparisons with the older way to find empty cells)
	for (Row = Grid_Size - 1; Row >= 0; Row--)
	{
		for (Column = Grid_Size - 1; Column >= 0; Column--)
		{
			if (Grid[Row][Column] == GRID_EMPTY_CELL_VALUE) CellsStackPush(Row, Column);
		}
	}
}

/** Read the next line from a grid file.
 * @param File The file to read from.
 * @param String_Destination Where to store the read data. The buffer must be at least CONFIGURATION_GRID_MAXIMUM_SIZE + 2 bytes long (+1 to allow to load a 17-characters string and to determinate that the line is too long, and +1 for terminating zero).
 * @return The size in characters of the read line (without terminating '\n').
 */
static int GridReadNextFileLine(FILE *File, char *String_Destination)
{
	int Length, New_Line_Character_Index;

	// Try to read the next line.
	if (fgets(String_Destination, CONFIGURATION_GRID_MAXIMUM_SIZE + 2, File) == NULL) return 0;

	// Remove any trailing '\n' or '\r'
	Length = strlen(String_Destination);
	New_Line_Character_Index = Length - 1; // The index in the string where the new line character can be
	if ((New_Line_Character_Index >= 0) && ((String_Destination[New_Line_Character_Index] == '\n') || (String_Destination[New_Line_Character_Index] == '\r')))
	{
		String_Destination[New_Line_Character_Index] = 0;
		Length--;
	}

	return Length;
}

/** Convert a grid character read from a text file into a program operable value.
 * @param Character The character's value read from the file.
 * @return GRID_EMPTY_CELL_VALUE if the cell is empty,
 * @return The corresponding numerical value if the value was recognized,
 * @return -1 if the character is not an hexadecimal digit or a dot.
 */
static int GridConvertCharacterToValue(char Character)
{
	// Is the character a digit ?
	if ((Character >= '0') && (Character <= '9')) return Character - '0';

	// Is the character an hexadecimal alphanumerical value ?
	if ((Character >= 'A') && (Character <= 'F')) return Character - 'A' + 10; // Plus 10 as the 'A' letter represents 10

	// Is the character an empty cell ?
	if (Character == '.') return GRID_EMPTY_CELL_VALUE;

	// Bad character
	return -1;
}

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void GridShow(void)
{
	unsigned int Row, Column;
	int Value;
	
	for (Row = 0; Row < Grid_Size; Row++)
	{
		for (Column = 0; Column < Grid_Size; Column++)
		{
			Value = Grid[Row][Column];
			if (Value == GRID_EMPTY_CELL_VALUE) printf(" . ");
			else printf("%2d ", Value + Grid_Display_Starting_Number);
		}
		putchar('\n');
	}
}

unsigned int GridGetCellMissingNumbers(unsigned int Cell_Row, unsigned int Cell_Column)
{
	unsigned int Bitmask_Missing_Numbers, Square_Index;
	
	// No need to check a filled cell
	if (Grid[Cell_Row][Cell_Column] != GRID_EMPTY_CELL_VALUE) return 0;
	
	// Determinate the index of the square where the cell is located
	Square_Index = GRID_GET_CELL_SQUARE_INDEX(Cell_Row, Cell_Column);
	
	// Find missing numbers simultaneously on the row, the column and the square of the cell
	Bitmask_Missing_Numbers = Bitmask_Rows[Cell_Row] & Bitmask_Columns[Cell_Column] & Bitmask_Squares[Square_Index];
	return Bitmask_Missing_Numbers;
}

int GridIsCorrectlyFilled(void)
{
	unsigned int Row, Column, Cell_Row, Cell_Column, Column_Start, Row_End, Column_End;
	int Number, Is_Number_Found[CONFIGURATION_GRID_MAXIMUM_SIZE];
	
	// Check each row correctness
	for (Row = 0; Row < Grid_Size; Row++)
	{
		// Reset boolean array of found numbers
		memset(Is_Number_Found, 0, sizeof(Is_Number_Found));
		
		// Find all filled numbers
		for (Column = 0; Column < Grid_Size; Column++)
		{
			Number = Grid[Row][Column];
			if (Number == GRID_EMPTY_CELL_VALUE) continue; // Ignore empty cells
			if (Is_Number_Found[Number]) return 0; // The number is present more than one time
			Is_Number_Found[Number] = 1;
		}
	}
	
	// Check each column correctness
	for (Column = 0; Column < Grid_Size; Column++)
	{
		// Reset boolean array of found numbers
		memset(Is_Number_Found, 0, sizeof(Is_Number_Found));
		
		// Find all filled numbers
		for (Row = 0; Row < Grid_Size; Row++)
		{
			Number = Grid[Row][Column];
			if (Number == GRID_EMPTY_CELL_VALUE) continue; // Ignore empty cells
			if (Is_Number_Found[Number]) return 0; // The number is present more than one time
			Is_Number_Found[Number] = 1;
		}
	}
	
	// Check each cell in each square
	for (Cell_Row = 0; Cell_Row < Grid_Size; Cell_Row++)
	{
		for (Cell_Column = 0; Cell_Column < Grid_Size; Cell_Column++)
		{
			// Reset boolean array of found numbers
			memset(Is_Number_Found, 0, sizeof(Is_Number_Found));
			
			// Determine coordinates of the square where the cell is located
			Row = (Cell_Row / Square_Height) * Square_Height;
			Column_Start = (Cell_Column / Square_Width) * Square_Width;
			Row_End = Row + Square_Height;
			Column_End = Column_Start + Square_Width;
			
			// Find numbers present into the square
			for ( ; Row < Row_End; Row++)
			{
				for (Column = Column_Start; Column < Column_End; Column++)
				{
					Number = Grid[Row][Column];
					if (Number == GRID_EMPTY_CELL_VALUE) continue; // Ignore empty cells
					if (Is_Number_Found[Number]) return 0; // The number is present more than one time
					Is_Number_Found[Number] = 1;
				}
			}
		}
	}
	return 1;
}

void GridSetCellValue(unsigned int Cell_Row, unsigned int Cell_Column, int Cell_Value)
{
	// Check coordinates in debug mode
	#ifdef DEBUG
		assert(Cell_Row < Grid_Size);
		assert(Cell_Column < Grid_Size);
	#endif
	Grid[Cell_Row][Cell_Column] = Cell_Value;
}

int GridLoadFromFile(char *String_File_Name, unsigned int *Pointer_Grid_Size)
{
	FILE *File;
	unsigned int Row, Column, Temp;
	char String_Line[CONFIGURATION_GRID_MAXIMUM_SIZE + 2] = {0};
	
	// Try to open the file
	File = fopen(String_File_Name, "rb");
	if (File == NULL) return -1;
	
	// Retrieve the grid size according to the length of the first line
	Grid_Size = GridReadNextFileLine(File, String_Line);
	if (Grid_Size > CONFIGURATION_GRID_MAXIMUM_SIZE)
	{
		fclose(File);
		#ifdef DEBUG
			printf("[%s] First line length is too long.\n", __FUNCTION__);
		#endif
		return -2;
	}

	// Check if the grid size can be handled by the solver
	switch (Grid_Size)
	{
		case 6:
			Square_Width = 3;
			Square_Height = 2;
			Grid_Display_Starting_Number = 1;
			break;

		case 9:
			Square_Width = 3;
			Square_Height = 3;
			Grid_Display_Starting_Number = 1;
			break;

		case 12:
			Square_Width = 4;
			Square_Height = 3;
			Grid_Display_Starting_Number = 1;
			break;

		case 16:
			Square_Width = 4;
			Square_Height = 4;
			Grid_Display_Starting_Number = 0;
			break;

		default:
			#ifdef DEBUG
				printf("[%s] Unrecognized grid size.\n", __FUNCTION__);
			#endif
			return -2;
	}
	
	// Compute number of squares on grid width and height
	Squares_Horizontal_Count = Grid_Size / Square_Width;
	Squares_Vertical_Count = Grid_Size / Square_Height;
	
	// Load grid
	for (Row = 0; Row < Grid_Size; Row++)
	{
		// Get each cell value
		for (Column = 0; Column < Grid_Size; Column++)
		{
			// Get the numerical value of each cell
			Temp = GridConvertCharacterToValue(String_Line[Column]);
			if ((Temp != GRID_EMPTY_CELL_VALUE) && (Temp >= Grid_Size))
			{
				#ifdef DEBUG
					printf("[%s] The read character value (%d) is too big for the grid size.\n", __FUNCTION__, Temp);
				#endif
				return -3;
			}
			if (Temp == (unsigned int) -1)
			{
				#ifdef DEBUG
					printf("[%s] A bad character was read.\n", __FUNCTION__);
				#endif
				return -3;
			}

			Grid[Row][Column] = Temp;
		}

		// Load the next line
		if (Row < Grid_Size - 1) // -1 because the first line was read before entering the loop
		{
			Temp = GridReadNextFileLine(File, String_Line);
			if (Temp != Grid_Size)
			{
				#ifdef DEBUG
					printf("[%s] The line %d has not the same length than the previous ones (%d).\n", __FUNCTION__, Row + 2, Temp); // +1 because the text editor starts displaying lines from 1, and +1 because the first line was already read (to get the grid size)
				#endif
				return -2;
			}
		}
	}

	// The grid was successfully loaded
	fclose(File);

	// Create first bitmasks
	GridGenerateInitialBitmasks();
	
	// Put the empty cells coordinates into the dedicated stack
	GridFillStackWithEmptyCells();
	*Pointer_Grid_Size = Grid_Size;
	return 0;
}

int GridGetEmptyCellsCount(void)
{
	unsigned int Row, Column;
	int Empty_Cells_Count = 0;
	
	for (Row = 0; Row < Grid_Size; Row++)
	{
		for (Column = 0; Column < Grid_Size; Column++)
		{
			if (Grid[Row][Column] == GRID_EMPTY_CELL_VALUE) Empty_Cells_Count++;
		}
	}
	return Empty_Cells_Count;
}

void GridRemoveCellMissingNumber(unsigned int Cell_Row, unsigned int Cell_Column, int Number)
{
	unsigned int New_Bitmask;
	
	New_Bitmask = ~(1 << Number);
	
	// Disable the Number bit in all relevant bitmasks
	Bitmask_Rows[Cell_Row] &= New_Bitmask;
	Bitmask_Columns[Cell_Column] &= New_Bitmask;
	Bitmask_Squares[GRID_GET_CELL_SQUARE_INDEX(Cell_Row, Cell_Column)] &= New_Bitmask;
}

void GridRestoreCellMissingNumber(unsigned int Cell_Row, unsigned int Cell_Column, int Number)
{
	unsigned int New_Bitmask;
	
	New_Bitmask = 1 << Number;
	
	// Disable the Number bit in all relevant bitmasks
	Bitmask_Rows[Cell_Row] |= New_Bitmask;
	Bitmask_Columns[Cell_Column] |= New_Bitmask;
	Bitmask_Squares[GRID_GET_CELL_SQUARE_INDEX(Cell_Row, Cell_Column)] |= New_Bitmask;
}

void GridSetDisplayStartingNumber(int Starting_Number)
{
	Grid_Display_Starting_Number = Starting_Number;
}

#ifdef DEBUG
	void GridShowBitmask(unsigned int Bitmask)
	{
		int i, j = 0;
		
		for (i = Grid_Size; i >= 0 ; i--)
		{
			// Show '1' or '0' according to bit value
			if (Bitmask & (1 << i)) putchar('1');
			else putchar('0');
				
			// Make 4-bit groups to ease reading
			j++;
			if (j == 4)
			{
				putchar(' ');
				j = 0;
			}
		}
		printf("\n");
	}

	void GridShowDifferences(int Color_Code)
	{
		static int Last_Grid[CONFIGURATION_GRID_MAXIMUM_SIZE][CONFIGURATION_GRID_MAXIMUM_SIZE];
		unsigned int Row, Column;
		int Has_Color_Changed, Current_Cell_Value;
		
		for (Row = 0; Row < Grid_Size; Row++)
		{
			for (Column = 0; Column < Grid_Size; Column++)
			{
				// Change terminal color if there is a difference between cells
				Has_Color_Changed = 0;
				Current_Cell_Value = Grid[Row][Column];
				if (Last_Grid[Row][Column] != Current_Cell_Value)
				{
					if (Color_Code == GRID_COLOR_CODE_BLUE) printf("\x1B[34m"); // VT100 escape sequence
					else printf("\x1B[31m"); // VT100 escape sequence
					Has_Color_Changed = 1;
				}
				
				if (Current_Cell_Value == GRID_EMPTY_CELL_VALUE) printf(" . ");
				else printf("%2d ", Current_Cell_Value + Grid_Display_Starting_Number);
				
				// Restore terminal color
				if (Has_Color_Changed) printf("\x1B[0m");
				
				// Update grid in the same time
				Last_Grid[Row][Column] = Current_Cell_Value;
			}
			putchar('\n');
		}
	}
#endif