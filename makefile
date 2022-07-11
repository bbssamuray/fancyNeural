
OBJS = main.cpp

CC = g++

COMPILER_FLAGS = -O2

LINKER_FLAGS = -lpthread -lraylib

OBJ_NAME = neural

start:
	$(MAKE) executable
#	$(MAKE) clean

executable: agent.o main.o breeder.o
	$(CC) agent.o main.o breeder.o $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)


agent.o:
	$(CC) $(COMPILER_FLAGS) -c agent.cpp

main.o:
	$(CC) $(COMPILER_FLAGS) -c main.cpp

breeder.o:
	$(CC) $(COMPILER_FLAGS) -c breeder.cpp


clean:
	rm main.o agent.o breeder.o
