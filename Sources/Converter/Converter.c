/** @file Converter.c
 * Convert e-sudoku web site sudoku to SudokuSolver format.
 * @author Adrien RICCIARDI
 * @version 1.0 : 26/12/2013
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** Size of the token buffer in bytes. */
#define BUFFER_SIZE 32

//-------------------------------------------------------------------------------------------------
// Private types
//-------------------------------------------------------------------------------------------------
typedef enum
{
	RESULT_READ_TOKEN_NO_ERROR, //! A token was successfully read.
	RESULT_READ_TOKEN_ERROR_BUFFER_OVERFLOW, //! The destination buffer overflowed before ';' was found.
	RESULT_READ_TOKEN_EOF //! The end of the file was reached before a ';' was found.
} TResultReadToken;

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
static FILE *File;
static char Buffer[BUFFER_SIZE], Grid[BUFFER_SIZE * BUFFER_SIZE];

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Read the file until the ';' character is found.
 * @return A TResultReadToken.
 */
static TResultReadToken ReadToken(void)
{
	int i;
	
	// Read up to BUFFER_SIZE characters
	for (i = 0; i < BUFFER_SIZE - 1; i++)
	{
		if (fread(&Buffer[i], 1, 1, File) != 1)
		{
			Buffer[i] = 0;
			return RESULT_READ_TOKEN_EOF;
		}
		
		// Exit when ';' is found
		if (Buffer[i] == ';')
		{
			Buffer[i + 1] = 0;
			return RESULT_READ_TOKEN_NO_ERROR;
		}
	}
	Buffer[i] = 0;
	return RESULT_READ_TOKEN_ERROR_BUFFER_OVERFLOW;	
}

/** Read the cell value from a token.
 * @return The cell value or -1 if an error occured.
 */
static int ReadCellValue(void)
{
	int i = 0;
	char Cell_Value;
	
	// Search for '"' character
	while ((Buffer[i] != '\"') && (Buffer[i] != 0)) i++;
	if (Buffer[i] == 0) return -1; // No '"' found
		
	// Extract cell value
	Cell_Value = Buffer[i + 1];
	
	// Convert to number
	if (Cell_Value == ' ') Cell_Value = 0; // Empty cell
	else if ((Cell_Value >= '0') && (Cell_Value <= '9')) Cell_Value -= '0';
	else if ((Cell_Value >= 'A') && (Cell_Value <= 'G')) Cell_Value = Cell_Value - 'A' + 10;
	else Cell_Value = -1;
	
	return Cell_Value;
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	TResultReadToken Result_Read_Token;
	int Cell_Value, i = 0, Grid_Size, Vertical_Squares_Count, Horizontal_Squares_Count, Row, Column;
	char *String_Input_File, *String_Output_File;
	
	// Check parameters
	if (argc != 6)
	{
		printf("Usage : %s InputFile OutputFile GridSize VerticalSquaresCount HorizontalSquaresCount\nThe input file must contain only the sudoku[0][0]=... line extracted from web page source code.\n", argv[0]);
		return -1;
	}
	String_Input_File = argv[1];
	String_Output_File = argv[2];
	Grid_Size = atoi(argv[3]);
	Vertical_Squares_Count = atoi(argv[4]);
	Horizontal_Squares_Count = atoi(argv[5]);
	
	// Open file
	File = fopen(String_Input_File, "r");
	if (File == NULL)
	{
		printf("Error : input file not found.\n");
		return -1;
	}
	
	while (1)
	{
		// Read next token
		Result_Read_Token = ReadToken();
		if (Result_Read_Token == RESULT_READ_TOKEN_ERROR_BUFFER_OVERFLOW)
		{
			printf("Error : token too large.\n");
			fclose(File);
			return -1;
		}
		else if (Result_Read_Token == RESULT_READ_TOKEN_EOF)
		{
			fclose(File);
			break;
		}
		#ifdef DEBUG
			printf("[DEBUG ReadToken] Token : %s\n", Buffer);
		#endif
		
		// Discard tokens starting with "solution"
		if (strncmp(Buffer, "solution", 8) == 0) continue;
		
		// Get cell value
		Cell_Value = ReadCellValue();
		if (Cell_Value == -1)
		{
			printf("Error : invalid cell value.\n");
			fclose(File);
			return -1;
		}
		#ifdef DEBUG
			printf("[DEBUG ReadCellValue] Value : %d\n", Cell_Value);
		#endif
		
		// Add to grid
		Grid[i] = Cell_Value;
		i++;
	}
	
	// Write output file
	File = fopen(String_Output_File, "w");
	if (File == NULL)
	{
		printf("Error : can't open output file.\n");
		return -1;
	}
	
	// Write grid format
	fprintf(File, "%d\n%d\n%d\n", Grid_Size, Vertical_Squares_Count, Horizontal_Squares_Count);
	
	for (Row = 0; Row < Grid_Size; Row++)
	{
		for (Column = 0; Column < Grid_Size; Column++) fprintf(File, "%02d ", Grid[Row * Grid_Size + Column]);
		fprintf(File, "\n");
	}
	fclose(File);
	
	return 0;
}
