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
#include <queue>

using namespace std;


// methods for gui handling
extern SDL_Texture* load_image(string, SDL_Renderer*);

extern int handle_suggest_mouse();
extern int handle_getting_suggested_mouse();
extern int handle_board_mouse();

extern void render_hand(SDL_Renderer*, vector<int>);
extern bool return_to_board();

extern void render_all_players(unordered_map<int, int>, SDL_Renderer*);
extern void push_banner(string, SDL_Renderer*, queue< SDL_Texture* > *, queue<SDL_Rect> *);

extern void load_all_media(SDL_Renderer*);
extern void fill_location_map();
extern bool in_room(int);



// declarations for textures and other media

extern const int SCREEN_WIDTH, SCREEN_HEIGHT, BOARD_WIDTH;
extern const int BANNER_HEIGHT;

extern const int suggest_y0, suggest_yf;
extern const int suggest1_x0, suggest1_xf;
extern const int suggest2_x0, suggest2_xf;
extern const int suggest3_x0, suggest3_xf;

extern const int accuse_y1, accuse_y2, accuse_y3, accuse_y4, accuse_y5, accuse_y6;


// coordinates for each board position
// the board is generally symmetric so a lot of quantities can be derived
extern const int board_start1, board_start2, board_start3;
extern const int room_width, hallway_width;
extern const int board_end1, board_end2, board_end3;
extern const int hall_start1, hall_start2, hall_start3;
extern const int hall_end1, hall_end2, hall_end3;




// textures for actual images
extern SDL_Texture* board;
extern SDL_Texture* suggest_background;
extern SDL_Texture* notify_background;
extern SDL_Texture* hand_background;

// for formatting text
extern TTF_Font* laser_font;
extern TTF_Font* blood_font;
extern SDL_Color silver, red;

extern SDL_Rect background_rect, notification_rect;
extern SDL_Rect suggest_rect1, suggest_rect2, suggest_rect3;



// maps to dynamically determine what to use
extern unordered_map<int, SDL_Texture*> card_image_map;

extern unordered_map<int, SDL_Rect> suggest_rect_map;
extern unordered_map<int, SDL_Rect> accuse_rect_map;

// track all player locations
extern unordered_map<int, int> player_locations;
