BINARIES_PATH = Binaries
INCLUDES_PATH = Includes
SOLVER_INCLUDES_PATH = $(INCLUDES_PATH)/Solver
OBJECTS_PATH = Objects
SOURCES_PATH = Sources
SOLVER_SOURCES_PATH = $(SOURCES_PATH)/Solver
CONVERTER_SOURCES_PATH = $(SOURCES_PATH)/Converter

CC = gcc
CCFLAGS = -W -Wall -I$(SOLVER_INCLUDES_PATH)

release: CCFLAGS += -O3 -fomit-frame-pointer -Wl,--strip-all -DNDEBUG
release: all

debug: CCFLAGS += -DDEBUG -g
debug: all

all:
	$(CC) $(CCFLAGS) $(SOLVER_SOURCES_PATH)/Cells_Stack.c $(SOLVER_SOURCES_PATH)/Grid.c $(SOLVER_SOURCES_PATH)/Main.c -o $(BINARIES_PATH)/Sudoku_Solver
	$(CC) $(CCFLAGS) $(CONVERTER_SOURCES_PATH)/Converter.c -o $(BINARIES_PATH)/Converter

clean:
	rm -f $(BINARIES_PATH)/*
