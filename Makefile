CC = gcc
CCFLAGS = -W -Wall

SOLVER_SOURCES_DIR = Sources/Solver
CONVERTER_SOURCES_DIR = Sources/Converter
OBJECTS_DIR = Objects
BINARIES_DIR = Binaries

SOLVER_OBJECTS = $(OBJECTS_DIR)/Main.o $(OBJECTS_DIR)/Grid.o $(OBJECTS_DIR)/Cells_Stack.o
CONVERTER_OBJECTS = $(OBJECTS_DIR)/Converter.o

release: CCFLAGS += -O3 -fexpensive-optimizations -ffast-math -Wl,--strip-all
release: all

debug: CCFLAGS += -DDEBUG -g
debug: all

all: $(SOLVER_OBJECTS) $(CONVERTER_OBJECTS)
	$(CC) $(CCFLAGS) $(SOLVER_OBJECTS) -o $(BINARIES_DIR)/Sudoku_Solver
	$(CC) $(CCFLAGS) $(CONVERTER_OBJECTS) -o $(BINARIES_DIR)/Converter

$(OBJECTS_DIR)/Main.o: $(SOLVER_SOURCES_DIR)/Main.c $(SOLVER_SOURCES_DIR)/Configuration.h $(SOLVER_SOURCES_DIR)/Grid.h
	$(CC) $(CCFLAGS) -c $(SOLVER_SOURCES_DIR)/Main.c -o $(OBJECTS_DIR)/Main.o

$(OBJECTS_DIR)/Grid.o: $(SOLVER_SOURCES_DIR)/Grid.c $(SOLVER_SOURCES_DIR)/Grid.h $(SOLVER_SOURCES_DIR)/Configuration.h $(SOLVER_SOURCES_DIR)/Cells_Stack.h
	$(CC) $(CCFLAGS) -c $(SOLVER_SOURCES_DIR)/Grid.c -o $(OBJECTS_DIR)/Grid.o

$(OBJECTS_DIR)/Cells_Stack.o: $(SOLVER_SOURCES_DIR)/Cells_Stack.c $(SOLVER_SOURCES_DIR)/Cells_Stack.h $(SOLVER_SOURCES_DIR)/Configuration.h
	$(CC) $(CCFLAGS) -c $(SOLVER_SOURCES_DIR)/Cells_Stack.c -o $(OBJECTS_DIR)/Cells_Stack.o

$(OBJECTS_DIR)/Converter.o: $(CONVERTER_SOURCES_DIR)/Converter.c
	$(CC) $(CCFLAGS) -c $(CONVERTER_SOURCES_DIR)/Converter.c -o $(OBJECTS_DIR)/Converter.o

clean:
	rm -f $(SOLVER_OBJECTS) $(CONVERTER_OBJECTS) $(BINARIES_DIR)/*
