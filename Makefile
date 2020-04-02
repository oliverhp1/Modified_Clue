#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
COMPILER_FLAGS = -Wall -std=c++11  # -v (for debugging) -c (no idea what that does, but it breaks everything)

#LINKER_FLAGS specifies the libraries we're linking against
#LINKER_FLAGS = -lSDL2 #-lSDL2_image -lSDL2_ttf -lSDL2_mixer

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = game

FILES = ClueLess.cpp Server.cpp Player.cpp GamePlay.cpp



all: $(OBJ_NAME)

$(OBJ_NAME): $(OBJS)
	$(CC) $(FILES) -o $(OBJ_NAME)   #$(LINKER_FLAGS)

# ClueLess.o: ClueLess.cpp Server.h
# 	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) ClueLess.cpp

# Server.o: Server.cpp Server.h
# 	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) Server.cpp



clean: 
	rm -rf *.o $(OBJ_NAME) 
