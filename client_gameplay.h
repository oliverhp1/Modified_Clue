#include "Server.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <netdb.h>
#include <stdio.h> 
#include <string.h>
#include <sstream>
#include <csignal>
#include <boost/algorithm/string.hpp>



extern string ping_server(fd_set*, int, int, timeval);

extern bool client_accusation(SDL_Renderer*, fd_set*, int, int, timeval);
extern bool validate_accusation(string);

extern void client_suggestion(SDL_Renderer*, fd_set*, int, int, int, timeval);
extern void confirm_suggestion(SDL_Renderer*, fd_set*, int, int, timeval);

extern string getting_suggested(string, SDL_Renderer*);