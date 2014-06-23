#!/bin/bash
# Do some sudoku resolution to find out what grids can't be solved.
# Author : Adrien RICCIARDI
# Version 0.0.1 : 23/12/2012
# Version 0.0.2 : 31/05/2013, used a for loop.
# Version 0.0.3 : 26/12/2013, formated "time" output.
export TIME="Time=%es Memory=%MkB"
PROGRAM="time ../Binaries/Sudoku_Solver"

function PrintFailure
{
	echo -e "\033[31m!!!!!!!!!!!!!"
	echo -e "!! FAILURE !!"
	echo -e "!!!!!!!!!!!!!\033[0m"
}

function PrintTotalSuccess
{
	echo -e "\033[32m#########################################"
	echo -e "## SUCCESS : all tests were successful ##"
	echo -e "#########################################\033[0m"
}

# Solve all present grids
for File in `find *.txt`
do
	$PROGRAM $File
	if [ $? != 0 ]
	then
		PrintFailure
		exit
	fi
done

PrintTotalSuccess
