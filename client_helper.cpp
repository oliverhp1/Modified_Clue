#include "client_helper.h"
#include "globals.h"

using namespace std;


// static attributes
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

// rectangles for each card in each situation
const int suggest_y0 = SCREEN_HEIGHT / 3;
const int suggest_yf = 2 * SCREEN_HEIGHT / 3;

const int suggest1_x0 = 1 * SCREEN_WIDTH / 7;
const int suggest1_xf = 2 * SCREEN_WIDTH / 7;

const int suggest2_x0 = 3 * SCREEN_WIDTH / 7;
const int suggest2_xf = 4 * SCREEN_WIDTH / 7;

const int suggest3_x0 = 5 * SCREEN_WIDTH / 7;
const int suggest3_xf = 6 * SCREEN_WIDTH / 7;






SDL_Rect background_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};


SDL_Rect suggest_rect1 = {suggest1_x0, suggest_y0, suggest1_xf - suggest1_x0, suggest_yf - suggest_y0};
SDL_Rect suggest_rect2 = {suggest2_x0, suggest_y0, suggest2_xf - suggest2_x0, suggest_yf - suggest_y0};
SDL_Rect suggest_rect3 = {suggest3_x0, suggest_y0, suggest3_xf - suggest3_x0, suggest_yf - suggest_y0};


unordered_map<int, SDL_Rect> suggest_rect_map;
unordered_map<int, SDL_Rect> accuse_rect_map;	// just for locations



SDL_Texture* board = NULL;
SDL_Texture* suggest_background = NULL;


// all cards' textures stored in here
unordered_map<int, SDL_Texture*> image_map;




// methods for gui handling

SDL_Texture* load_image(string path, SDL_Renderer* renderer){
	// free();
	SDL_Texture* tmp_texture = NULL;
	SDL_Surface* load_surface = IMG_Load(path.c_str());

	if (load_surface == NULL){
		printf("load %s error: %s\n", path.c_str(), IMG_GetError());
	}
	else{
		tmp_texture = SDL_CreateTextureFromSurface(renderer, load_surface);
		SDL_FreeSurface(load_surface);
	}

	if (tmp_texture == NULL){
		cerr << "Error: unable to load texture" << endl;
	}

	// sTexture = tmp_texture;
	return tmp_texture;

}


// this will populate all the textures defined in client_helper
void load_all_media(SDL_Renderer* renderer){
	board = NULL;
	suggest_background = NULL;

    board = load_image("images/board.png", renderer);
	if (board == NULL){
		cerr << "Couldn't load board... exiting\n" << endl;
		exit(1);
	}

	// LOAD ALL OTHER IMAGES HERE 
	suggest_background = load_image("images/suggest.png", renderer);
	if (suggest_background == NULL){
		cerr << "Couldn't load suggest... exiting\n" << endl;
		exit(1);
	}


	// all cards in map
	for (int i = 1; i <= 21; i++){
		// temp: only load available cards
		if (!((i == 1) || (i == 7) || (i == 9) || (i == 12) || (i == 14) || (i == 15) || (i == 18 ) || (i == 21))) continue;

		SDL_Texture* temp_texture = NULL;
		temp_texture = load_image("images/" + to_string(i) + ".png", renderer);

		image_map[i] = temp_texture;
	}
	

	suggest_rect_map[1] = suggest_rect1;
	suggest_rect_map[2] = suggest_rect2;
	suggest_rect_map[3] = suggest_rect3;
	// suggest_rect_map[4] = suggest_rect4;
	// suggest_rect_map[5] = suggest_rect5;
	// suggest_rect_map[6] = suggest_rect6;
}





/*
 * this method returns an int depending on where on the board the mouse is
 * it is used in tandem with a mouse click event
 * to decide what gameplay logic to execute
 * return: string corresponding to board action
 *	using a magic number or enum works as well, but this is easier
 */
string handle_board_mouse(){
	// can use SCREEN_WIDTH and SCREEN_HEIGHT if needed
	int mouse_x = 0;
	int mouse_y = 0;		// mouse coordinates
	
	string result = "empty space";

	SDL_GetMouseState(&mouse_x, &mouse_y);

	// cout << "x, y = " << mouse_x << ", " << mouse_y << endl;	// for debugging purposes
	// get actions
	if ((mouse_x > 26) && (mouse_x < 157) && (mouse_y > 14) && (mouse_y < 48)){
		result = navigate_str;
	}
	else if ((mouse_x > 26) && (mouse_x < 157) && (mouse_y > 58) && (mouse_y < 91)){
		result = suggest_str;
	}
	else if ((mouse_x > 573) && (mouse_x < 705) && (mouse_y > 15) && (mouse_y < 48)){
		result = accuse_str;
	}
	else if ((mouse_x > 573) && (mouse_x < 705) && (mouse_y > 58) && (mouse_y < 91)){
		result = pass_str;
	}

	else if ((mouse_x > 142) && (mouse_x < 229) && (mouse_y > 123) && (mouse_y < 217)){
		result = study;
	}
	else if ((mouse_x > 316) && (mouse_x < 404) && (mouse_y > 123) && (mouse_y < 217)){
		result = hall;
	}
	else if ((mouse_x > 492) && (mouse_x < 577) && (mouse_y > 123) && (mouse_y < 217)){
		result = lounge;
	}
	else if ((mouse_x > 142) && (mouse_x < 229) && (mouse_y > 315) && (mouse_y < 405)){
		result = library;
	}
	else if ((mouse_x > 142) && (mouse_x < 229) && (mouse_y > 123) && (mouse_y < 217)){
		// TOP RIGHT
		result = "4";
	}
	else if ((mouse_x > 142) && (mouse_x < 229) && (mouse_y > 123) && (mouse_y < 217)){
		// TOP RIGHT
		result = "4";
	}
	else if ((mouse_x > 142) && (mouse_x < 229) && (mouse_y > 123) && (mouse_y < 217)){
		// TOP RIGHT
		result = "4";
	}
	else if ((mouse_x > 142) && (mouse_x < 229) && (mouse_y > 123) && (mouse_y < 217)){
		// TOP RIGHT
		result = "4";
	}
	else if ((mouse_x > 142) && (mouse_x < 229) && (mouse_y > 123) && (mouse_y < 217)){
		// TOP RIGHT
		result = "4";
	}
	

	// can use these for ease if we're custom rendering stuff
	// else if ( (mouse_x > TextTexture_R[Main_Instruct].x) && (mouse_x < TextTexture_R[Main_Instruct].x + TextTexture_R[Main_Instruct].w) && (mouse_y > TextTexture_R[Main_Instruct].y) && (mouse_y < TextTexture_R[Main_Instruct].y + TextTexture_R[Main_Instruct].h)){
	// 	overInstruct = true;
	// 	result = 2;
	// }
	// else if ( (mouse_x > TextTexture_R[Main_Quit].x) && (mouse_x < TextTexture_R[Main_Quit].x + TextTexture_R[Main_Quit].w) && (mouse_y > TextTexture_R[Main_Quit].y) && (mouse_y < TextTexture_R[Main_Quit].y + TextTexture_R[Main_Quit].h)){
	// 	overQuit = true;
	// 	result = 3;
	// }

	// can use these to render highlight sections over whatever portion the mouse is over
	// ideally only do that if it's a valid move. but that's not very straightforward
	// SDL_RenderCopyEx(gRenderer, TextTextures[Main_Asteroids], NULL, &TextTexture_R[Main_Asteroids], 0, NULL, SDL_FLIP_NONE);

	// if (!overPlay){
	// 	SDL_RenderCopyEx(gRenderer, TextTextures[Main_Play], NULL, &TextTexture_R[Main_Play], 0, NULL, SDL_FLIP_NONE);
	// }
	// else{
	// 	SDL_RenderCopyEx(gRenderer, TextTextures[Main_Play_H], NULL, &TextTexture_R[Main_Play_H], 0, NULL, SDL_FLIP_NONE);
	// }


	// this will make it very quick to figure out what pixels to click 
	// result = "(x, y) = (" + to_string(mouse_x) + ", " + to_string(mouse_y) + ")";


	return result;
}


// this one handles the suggestion screen
// to avoid duplicate work, just return an int
// and use the same positions for players and weapons


// TODO: CAN USE THIS FOR ACCUSE TOO, just handle the int where this method is called
// TODO: STICK ALL THESE COORDINATES IN VARIABLES??
// for suggest, yes. for the general board, probably not necessary
int handle_suggest_mouse(){
	// can use SCREEN_WIDTH and SCREEN_HEIGHT if needed
	int mouse_x = 0;
	int mouse_y = 0;		// mouse coordinates
	
	int result = -1;

	SDL_GetMouseState(&mouse_x, &mouse_y);

	// get actions
	if ((mouse_x > 93) && (mouse_x < 203) && (mouse_y > 88) && (mouse_y < 209)){
		result = 1;
	}
	else if ((mouse_x > 306) && (mouse_x < 414) && (mouse_y > 88) && (mouse_y < 209)){
		result = 2;
	}
	else if ((mouse_x > 509) && (mouse_x < 618) && (mouse_y > 88) && (mouse_y < 209)){
		result = 3;
	}
	else if ((mouse_x > 93) && (mouse_x < 203) && (mouse_y > 286) && (mouse_y < 395)){
		result = 4;
	}
	else if ((mouse_x > 306) && (mouse_x < 414) && (mouse_y > 286) && (mouse_y < 395)){
		result = 5;
	}
	else if ((mouse_x > 509) && (mouse_x < 618) && (mouse_y > 286) && (mouse_y < 395)){
		result = 6;
	}

	return result;
}

// getting suggested: uses different coordinates
int handle_getting_suggested_mouse(){
	int mouse_x = 0;
	int mouse_y = 0;		// mouse coordinates
	
	int result = -1;

	SDL_GetMouseState(&mouse_x, &mouse_y);

	// y coordinate is all the same
	if ((mouse_y > suggest_y0) && (mouse_y < suggest_yf)){
		if ((mouse_x > suggest1_x0) && (mouse_x < suggest1_xf)){
			result = 1;
		}
		else if ((mouse_x > suggest2_x0) && (mouse_x < suggest2_xf)){
			result = 2;
		}
		else if ((mouse_x > suggest3_x0) && (mouse_x < suggest3_xf)){
			result = 3;
		}
	}
	
	return result;
}

