#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# unfortunately fd_set doesn't work with c++11 and after, so we will not use this
COMPILER_FLAGS = -std=c++11 # -Wall -v (for debugging) -c (no idea what that does, but it breaks everything)

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf #-lSDL2_mixer

#OBJ_NAME specifies the name of our exectuable
SERVER_NAME = server
CLIENT_NAME = client

SERVER_FILES = ClueLess.cpp Server.cpp Player.cpp GamePlay.cpp globals.cpp
CLIENT_FILES = Client.cpp globals.cpp Player.cpp client_gui.cpp client_gameplay.cpp


all: $(SERVER_NAME) $(CLIENT_NAME)

$(SERVER_NAME): $(OBJS) 
	
	$(CC) $(SERVER_FILES) -o $(SERVER_NAME)

$(CLIENT_NAME): $(OBJS)
	$(CC) $(LINKER_FLAGS) $(CLIENT_FILES) -o $(CLIENT_NAME)


#$(CC) $(COMPILER_FLAGS) $(FILES) -o $(OBJ_NAME)   

# ClueLess.o: ClueLess.cpp Server.h
# 	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) ClueLess.cpp

# Server.o: Server.cpp Server.h
# 	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) Server.cpp




clean: 
	rm -rf *.o $(SERVER_NAME) $(CLIENT_NAME)
