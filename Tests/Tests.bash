#!/bin/bash
# Do some sudoku resolution to find out what grids can't be solved.
# Author : Adrien RICCIARDI
# Version 0.0.1 : 23/12/2012
# Version 0.0.2 : 31/05/2013, used a for loop.
# Version 0.0.3 : 26/12/2013, formated "time" output.
# Version 0.0.4 : 29/08/2014, tests are now launched by grid size order.
export TIME="Time=%es Memory=%MkB"
Program="time ../Binaries/Sudoku_Solver"

function PrintFailure
{
	echo -e "\033[31m!!!!!!!!!!!!!"
	echo -e "!! FAILURE !!"
	echo -e "!!!!!!!!!!!!!\033[0m"
}

function PrintSuccess
{
	echo -e "\033[32m#########################################"
	echo -e "## SUCCESS : all tests were successful ##"
	echo -e "#########################################\033[0m"
}

function SolveList
{
	for File in $Files_List
	do
		$Program $Grid_Display_Starting_Number $File
		if [ $? != 0 ]
		then
			PrintFailure
			exit
		fi
	done
}

# Solve all 6x6 grids
Files_List=`find 6x6_*.txt`
Grid_Display_Starting_Number=1
SolveList

# Solve all 9x9 grids
Files_List=`find 9x9_*.txt`
$Grid_Display_Starting_Number=1
SolveList

# Solve all 12x12 grids
Files_List=`find 12x12_*.txt`
Grid_Display_Starting_Number=1
SolveList

# Solve all 16x16 grids
Files_List=`find 16x16_*.txt`
Grid_Display_Starting_Number=0
SolveList

PrintSuccess
