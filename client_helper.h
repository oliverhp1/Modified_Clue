#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>
#include <unordered_map>

#include <stdio.h>  
#include <string.h>
#include <stdlib.h>  
#include <vector> 
#include <sstream>
#include <sys/time.h>


using namespace std;


// methods for gui handling
extern SDL_Texture* load_image(string, SDL_Renderer*);

extern int handle_suggest_mouse();
extern int handle_getting_suggested_mouse();
extern string handle_board_mouse();

extern string ping_server(fd_set*, int, int, timeval);

extern void load_all_media(SDL_Renderer*);




// declarations for textures and other media

extern const int SCREEN_WIDTH, SCREEN_HEIGHT;


extern const int suggest_y0, suggest_yf;
extern const int suggest1_x0, suggest1_xf;
extern const int suggest2_x0, suggest2_xf;
extern const int suggest3_x0, suggest3_xf;


extern SDL_Texture* board;
extern SDL_Texture* suggest_background;

extern SDL_Rect background_rect;
extern SDL_Rect suggest_rect1, suggest_rect2, suggest_rect3;



// maps to dynamically determine what to use
extern unordered_map<int, SDL_Texture*> card_image_map;

extern unordered_map<int, SDL_Rect> suggest_rect_map;
extern unordered_map<int, SDL_Rect> accuse_rect_map;
