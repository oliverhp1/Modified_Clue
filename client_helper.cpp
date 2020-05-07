#include "client_helper.h"
#include "globals.h"

using namespace std;


// static attributes
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;


// coordinates for each card when getting suggested
const int suggest_y0 = SCREEN_HEIGHT / 3;
const int suggest_yf = 2 * SCREEN_HEIGHT / 3;
const int suggest1_x0 = 3 * SCREEN_WIDTH / 28;
const int suggest1_xf = 9 * SCREEN_WIDTH / 28;
const int suggest2_x0 = 11 * SCREEN_WIDTH / 28;
const int suggest2_xf = 17 * SCREEN_WIDTH / 28;
const int suggest3_x0 = 19 * SCREEN_WIDTH / 28;
const int suggest3_xf = 25 * SCREEN_WIDTH / 28;

// coordinates for each card in suggest/accuse
const int accuse_y1 = 5 * SCREEN_HEIGHT / 30;
const int accuse_y2 = 11 * SCREEN_HEIGHT / 30;
const int accuse_y3 = 13 * SCREEN_HEIGHT / 30;
const int accuse_y4 = 19 * SCREEN_HEIGHT / 30;
const int accuse_y5 = 21 * SCREEN_HEIGHT / 30;
const int accuse_y6 = 27 * SCREEN_HEIGHT / 30;
const int accuse_x1 = 2 * SCREEN_HEIGHT / 20;
const int accuse_x2 = 5 * SCREEN_HEIGHT / 20;
const int accuse_x3 = 17 * SCREEN_HEIGHT / 40;
const int accuse_x4 = 23 * SCREEN_HEIGHT / 40;
const int accuse_x5 = 15 * SCREEN_HEIGHT / 20;
const int accuse_x6 = 18 * SCREEN_HEIGHT / 20;


// coordinates for each board position
// the board is generally symmetric so a lot of quantities can be derived
const int board_start1 = 142;
const int board_start2 = 316;
const int board_start3 = 490;

const int room_width = 87;	// rooms are square
const int player_width = room_width / 5;	// players are too
const int hallway_width = 30;	// hallways are not
const int hallway_length = room_width;	

const int board_end1 = board_start1 + room_width;
const int board_end2 = board_start2 + room_width;
const int board_end3 = board_start3 + room_width;

const int hall_start1 = board_start1 + hallway_width;
const int hall_start2 = board_start2 + hallway_width;
const int hall_start3 = board_start3 + hallway_width;

const int hall_end1 = hall_start1 + hallway_width;
const int hall_end2 = hall_start2 + hallway_width;
const int hall_end3 = hall_start3 + hallway_width;




// common rendering positions
SDL_Rect background_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
SDL_Rect notification_rect = {
	SCREEN_WIDTH / 10, 92 * SCREEN_HEIGHT / 100, 8 * SCREEN_WIDTH / 10, 6 * SCREEN_HEIGHT / 100
};

SDL_Rect suggest_rect1 = {suggest1_x0, suggest_y0, suggest1_xf - suggest1_x0, suggest_yf - suggest_y0};
SDL_Rect suggest_rect2 = {suggest2_x0, suggest_y0, suggest2_xf - suggest2_x0, suggest_yf - suggest_y0};
SDL_Rect suggest_rect3 = {suggest3_x0, suggest_y0, suggest3_xf - suggest3_x0, suggest_yf - suggest_y0};

SDL_Rect accuse_rect1 = {accuse_x1, accuse_y1, accuse_x2 - accuse_x1, accuse_y2 - accuse_y1};
SDL_Rect accuse_rect2 = {accuse_x3, accuse_y1, accuse_x4 - accuse_x3, accuse_y2 - accuse_y1};
SDL_Rect accuse_rect3 = {accuse_x5, accuse_y1, accuse_x6 - accuse_x5, accuse_y2 - accuse_y1};
SDL_Rect accuse_rect4 = {accuse_x1, accuse_y3, accuse_x2 - accuse_x1, accuse_y4 - accuse_y3};
SDL_Rect accuse_rect5 = {accuse_x3, accuse_y3, accuse_x4 - accuse_x3, accuse_y4 - accuse_y3};
SDL_Rect accuse_rect6 = {accuse_x5, accuse_y3, accuse_x6 - accuse_x5, accuse_y4 - accuse_y3};
SDL_Rect accuse_rect7 = {accuse_x1, accuse_y5, accuse_x2 - accuse_x1, accuse_y6 - accuse_y5};
SDL_Rect accuse_rect8 = {accuse_x3, accuse_y5, accuse_x4 - accuse_x3, accuse_y6 - accuse_y5};
SDL_Rect accuse_rect9 = {accuse_x5, accuse_y5, accuse_x6 - accuse_x5, accuse_y6 - accuse_y5};



// store positions in maps for easy access
unordered_map<int, SDL_Rect> suggest_rect_map;	// for getting suggested
unordered_map<int, SDL_Rect> accuse_rect_map;	// for suggesting and accusing
unordered_map< int, vector<int> > player_render_map;	// for rendering players


// textures for rendering
SDL_Texture* board = NULL;
SDL_Texture* suggest_background = NULL;
SDL_Texture* notify_background = NULL;
SDL_Texture* hand_background = NULL;


TTF_Font* laser_font = NULL;
TTF_Font* blood_font = NULL;

SDL_Color silver = {192, 192, 192};	// silver
SDL_Color red = {139, 0, 0};		// dark red


// all cards' textures stored in here
unordered_map<int, SDL_Texture*> card_image_map;
unordered_map<int, SDL_Texture*> player_icon_map;

// track all player locations
unordered_map<int, int> player_locations;







// render hand until player chooses to go back to board
void render_hand(SDL_Renderer* renderer, vector<int> hand){
	SDL_Event e;
	bool done = false;

	// render relevant cards until exit
	while (!done){
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, hand_background, NULL, &background_rect);
		for (int i = 0; i < hand.size(); i++){
			SDL_RenderCopy(renderer, card_image_map[hand[i]], NULL, &accuse_rect_map[i + 1]);	
		}
		SDL_RenderPresent(renderer);

		done = return_to_board();

		while (SDL_PollEvent(&e)){
	        if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON_LEFT)){
	        	if (done){
	        		return;
	        	}
	        }
	    }

	    done = false;
	}
	cout << "broken logic in render_hand" << endl;
	return;
}




// THE REMAINDER OF METHODS ARE FOR GUI HANDLING

// load a single image from file to texture
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


// this will populate all the textures we want
// and store them in easily accessible places
void load_all_media(SDL_Renderer* renderer){
	board = NULL;
	suggest_background = NULL;
	notify_background = NULL;
	hand_background = NULL;


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

	// for notifications at bottom of screen
	notify_background = load_image("images/notify.png", renderer);
	if (notify_background == NULL){
		cerr << "Couldn't load notify background... exiting\n" << endl;
		exit(1);
	}

	hand_background = load_image("images/hand.png", renderer);
	if (hand_background == NULL){
		cerr << "Couldn't load hand image... exiting\n" << endl;
		exit(1);
	}



	// load all cards into map
	for (int i = 1; i <= 21; i++){
		SDL_Texture* temp_texture = NULL;
		temp_texture = load_image("images/" + to_string(i) + ".png", renderer);

		card_image_map[i] = temp_texture;
	}

	// load all icons into map
	for (int i = 1; i <= 6; i++){
		SDL_Texture* temp_texture = NULL;
		temp_texture = load_image("images/" + to_string(i) + "_icon.png", renderer);

		player_icon_map[i] = temp_texture;
	}


	// different texts
	laser_font = TTF_OpenFont("fonts/laser.ttf", 3 * SCREEN_HEIGHT / 100);
	if (laser_font == NULL){
		printf("couldn't load laser font, error: %s\n", TTF_GetError());
		exit(1);
	}

	blood_font = TTF_OpenFont("fonts/bloody.ttf", 3 * SCREEN_HEIGHT / 100);
	if (blood_font == NULL){
		printf("couldn't load bloody font, error: %s\n", TTF_GetError());
		exit(1);
	}

	
	// where to render cards when getting suggested
	suggest_rect_map[1] = suggest_rect1;
	suggest_rect_map[2] = suggest_rect2;
	suggest_rect_map[3] = suggest_rect3;

	// where to render cards when making suggestion/accusation
	accuse_rect_map[1] = accuse_rect1;
	accuse_rect_map[2] = accuse_rect2;
	accuse_rect_map[3] = accuse_rect3;
	accuse_rect_map[4] = accuse_rect4;
	accuse_rect_map[5] = accuse_rect5;
	accuse_rect_map[6] = accuse_rect6;
	accuse_rect_map[7] = accuse_rect7;
	accuse_rect_map[8] = accuse_rect8;
	accuse_rect_map[9] = accuse_rect9;
}






// this will render all players
// given a map of locations
// assumption: only one player per hallway
// 	and up to 6 in a room
void render_all_players(unordered_map<int, int> player_locations,
		SDL_Renderer* renderer){
	// player 0: start at (1/10, 1/10)
	// player 1: start at (4/10, 1/10)
	// player 2: start at (7/10, 1/10)

	// player 4: start at (1/10, 4/10)
	// player 5: start at (4/10, 4/10)
	// player 6: start at (7/10, 4/10)
	int tmp_x, tmp_y, tmp_location;


	for (int id = 0; id < 6; id++){
		tmp_location = player_locations[id];

		tmp_x = player_render_map[tmp_location][0];
		tmp_y = player_render_map[tmp_location][1];

		if (in_room(tmp_location)){
			// players 0-2 are in the top row 
			tmp_x += (3 * (id % 3) + 1) * room_width / 10;
			tmp_y += room_width / 10;

			// players 3-5 are in the bottom row
			if (id >= 3){
				tmp_y += 3 * room_width / 10;
			}
		}

		SDL_Rect temp_rect = {tmp_x, tmp_y, player_width, player_width};

		SDL_RenderCopy(renderer, player_icon_map[id + 1], NULL, &temp_rect);
	}

	// ** these are all relative to the top left corner of the room
	// for hallways, stick directly in the middle
}






// helper map for rendering players
// for hallways, (x, y) for top left corner of player render
	// we want to center the players
// for rooms, (x, y) for top left of the room
	// we want to render the players offset so they don't overlap
void fill_location_map(){
	// top row
	player_render_map[1].push_back(board_start1);
	player_render_map[1].push_back(board_start1);
	player_render_map[2].push_back(board_end1 + (hallway_length / 2) - (player_width / 2));
	player_render_map[2].push_back(hall_start1 + (hallway_width / 2) - (player_width / 2));
	player_render_map[3].push_back(board_start2);
	player_render_map[3].push_back(board_start1);
	player_render_map[4].push_back(board_end2 + (hallway_length / 2) - (player_width / 2));
	player_render_map[4].push_back(hall_start1 + (hallway_width / 2) - (player_width / 2));
	player_render_map[5].push_back(board_start3);
	player_render_map[5].push_back(board_start1);

	// top vertical halls
	player_render_map[6].push_back(hall_start1 + (hallway_width / 2) - (player_width / 2));
	player_render_map[6].push_back(board_end1 + (hallway_length / 2) - (player_width / 2));
	player_render_map[7].push_back(hall_start2 + (hallway_width / 2) - (player_width / 2));
	player_render_map[7].push_back(board_end1 + (hallway_length / 2) - (player_width / 2));
	player_render_map[8].push_back(hall_start3 + (hallway_width / 2) - (player_width / 2));
	player_render_map[8].push_back(board_end1 + (hallway_length / 2) - (player_width / 2));

	// middle row
	player_render_map[9].push_back(board_start1);
	player_render_map[9].push_back(board_start2);
	player_render_map[10].push_back(board_end1 + (hallway_length / 2) - (player_width / 2));
	player_render_map[10].push_back(hall_start2 + (hallway_width / 2) - (player_width / 2));
	player_render_map[11].push_back(board_start2);
	player_render_map[11].push_back(board_start2);
	player_render_map[12].push_back(board_end2 + (hallway_length / 2) - (player_width / 2));
	player_render_map[12].push_back(hall_start2 + (hallway_width / 2) - (player_width / 2));
	player_render_map[13].push_back(board_start3);
	player_render_map[13].push_back(board_start2);

	// bottom vertical halls
	player_render_map[14].push_back(hall_start1 + (hallway_width / 2) - (player_width / 2));
	player_render_map[14].push_back(board_end2 + (hallway_length / 2) - (player_width / 2));
	player_render_map[15].push_back(hall_start2 + (hallway_width / 2) - (player_width / 2));
	player_render_map[15].push_back(board_end2 + (hallway_length / 2) - (player_width / 2));
	player_render_map[16].push_back(hall_start3 + (hallway_width / 2) - (player_width / 2));
	player_render_map[16].push_back(board_end2 + (hallway_length / 2) - (player_width / 2));

	// last row
	player_render_map[17].push_back(board_start1);
	player_render_map[17].push_back(board_start3);
	player_render_map[18].push_back(board_end1 + (hallway_length / 2) - (player_width / 2));
	player_render_map[18].push_back(hall_start3 + (hallway_width / 2) - (player_width / 2));
	player_render_map[19].push_back(board_start2);
	player_render_map[19].push_back(board_start3);
	player_render_map[20].push_back(board_end2 + (hallway_length / 2) - (player_width / 2));
	player_render_map[20].push_back(hall_start3 + (hallway_width / 2) - (player_width / 2));
	player_render_map[21].push_back(board_start3);
	player_render_map[21].push_back(board_start3);


	// starting blocks
	player_render_map[-1].push_back(board_end2 + (hallway_length / 2) - (player_width / 2));
	player_render_map[-1].push_back(board_start1 - hallway_width);
	player_render_map[-2].push_back(board_start1 - hallway_width);
	player_render_map[-2].push_back(board_end1 + (hallway_length / 2) - (player_width / 2));
	player_render_map[-3].push_back(board_start1 - hallway_width);
	player_render_map[-3].push_back(board_end2 + (hallway_length / 2) - (player_width / 2));
	player_render_map[-4].push_back(board_end1 + (hallway_length / 2) - (player_width / 2));
	player_render_map[-4].push_back(board_end3 + hallway_width);
	player_render_map[-5].push_back(board_end2 + (hallway_length / 2) - (player_width / 2));
	player_render_map[-5].push_back(board_end3 + hallway_width);
	player_render_map[-6].push_back(board_end3 + hallway_width);
	player_render_map[-6].push_back(board_end1 + (hallway_length / 2) - (player_width / 2));
}





/*
 * this method returns an int depending on where on the board the mouse is
 * it is used in tandem with a mouse click event
 * to decide what gameplay logic to execute
 * return: string corresponding to board action
 *	using a magic number or enum works as well, but this is easier
 */
int handle_board_mouse(){
	// can use SCREEN_WIDTH and SCREEN_HEIGHT if needed
	int mouse_x = 0;
	int mouse_y = 0;		// mouse coordinates
	
	int result = 36;	// return this if player clicked on nothing

	SDL_GetMouseState(&mouse_x, &mouse_y);

	// cout << "x, y = " << mouse_x << ", " << mouse_y << endl;	// for debugging purposes

	// get actions
	if ((mouse_x > 157) && (mouse_x < 287) && (mouse_y > 58) && (mouse_y < 91)){
		result = 30;
	}
	else if ((mouse_x > 26) && (mouse_x < 157) && (mouse_y > 14) && (mouse_y < 48)){
		result = 31;
	}
	else if ((mouse_x > 26) && (mouse_x < 157) && (mouse_y > 58) && (mouse_y < 91)){
		result = 32;
	}
	else if ((mouse_x > 573) && (mouse_x < 705) && (mouse_y > 15) && (mouse_y < 48)){
		result = 33;
	}
	else if ((mouse_x > 573) && (mouse_x < 705) && (mouse_y > 58) && (mouse_y < 91)){
		result = 34;
	}
	else if ((mouse_x > 157) && (mouse_x < 287) && (mouse_y > 14) && (mouse_y < 48)){
		result = 35;
	}

	
	// get rooms
	else if ((mouse_x > board_start1) && (mouse_x < board_end1) 
			&& (mouse_y > board_start1) && (mouse_y < board_end1)){
		result = 1;
	}
	else if ((mouse_x > board_start2) && (mouse_x < board_end2)
			&& (mouse_y > board_start1) && (mouse_y < board_end1)){
		result = 3;
	}
	else if ((mouse_x > board_start3) && (mouse_x < board_end3) 
			&& (mouse_y > board_start1) && (mouse_y < board_end1)){
		result = 5;
	}
	else if ((mouse_x > board_start1) && (mouse_x < board_end1) 
			&& (mouse_y > board_start2) && (mouse_y < board_end2)){
		result = 9;
	}
	else if ((mouse_x > board_start2) && (mouse_x < board_end2)
			&& (mouse_y > board_start2) && (mouse_y < board_end2)){
		result = 11;
	}
	else if ((mouse_x > board_start3) && (mouse_x < board_end3) 
			&& (mouse_y > board_start2) && (mouse_y < board_end2)){
		result = 13;
	}
	else if ((mouse_x > board_start1) && (mouse_x < board_end1)
			&& (mouse_y > board_start3) && (mouse_y < board_end3)){
		result = 17;
	}
	else if ((mouse_x > board_start2) && (mouse_x < board_end2)
			&& (mouse_y > board_start3) && (mouse_y < board_end3)){
		result = 19;
	}
	else if ((mouse_x > board_start3) && (mouse_x < board_end3) 
			&& (mouse_y > board_start3) && (mouse_y < board_end3)){
		result = 21;
	}

	// now for hallways. 
	// handle the 6 horizontal ones first
	else if ((mouse_x > board_end1) && (mouse_x < board_start2) 
			&& (mouse_y > hall_start1) && (mouse_y < hall_end1)){
		result = 2;
	}
	else if ((mouse_x > board_end2) && (mouse_x < board_start3) 
			&& (mouse_y > hall_start1) && (mouse_y < hall_end1)){
		result = 4;
	}

	else if ((mouse_x > board_end1) && (mouse_x < board_start2) 
			&& (mouse_y > hall_start2) && (mouse_y < hall_end2)){
		result = 10;
	}
	else if ((mouse_x > board_end2) && (mouse_x < board_start3) 
			&& (mouse_y > hall_start2) && (mouse_y < hall_end2)){
		result = 12;
	}

	// PAIR
	else if ((mouse_x > board_end1) && (mouse_x < board_start2) 
			&& (mouse_y > hall_start3) && (mouse_y < hall_end3)){
		result = 18;
	}
	else if ((mouse_x > board_end2) && (mouse_x < board_start3) 
			&& (mouse_y > hall_start3) && (mouse_y < hall_end3)){
		result = 20;
	}


	// then vertical ones: it's symmetric so just flip x and y
	else if ((mouse_y > board_end1) && (mouse_y < board_start2) 
			&& (mouse_x > hall_start1) && (mouse_x < hall_end1)){
		result = 6;
	}
	else if ((mouse_y > board_end2) && (mouse_y < board_start3) 
			&& (mouse_x > hall_start1) && (mouse_x < hall_end1)){
		result = 14;
	}

	else if ((mouse_y > board_end1) && (mouse_y < board_start2) 
			&& (mouse_x > hall_start2) && (mouse_x < hall_end2)){
		result = 7;
	}
	else if ((mouse_y > board_end2) && (mouse_y < board_start3) 
			&& (mouse_x > hall_start2) && (mouse_x < hall_end2)){
		result = 15;
	}

	else if ((mouse_y > board_end1) && (mouse_y < board_start2) 
			&& (mouse_x > hall_start3) && (mouse_x < hall_end3)){
		result = 8;
	}
	else if ((mouse_y > board_end2) && (mouse_y < board_start3) 
			&& (mouse_x > hall_start3) && (mouse_x < hall_end3)){
		result = 16;
	}
	


	// this will make it very quick to figure out what pixels to click 
	// result = "(x, y) = (" + to_string(mouse_x) + ", " + to_string(mouse_y) + ")";


	return result;
}



// this one handles the suggestion and accuse screens
// to avoid duplicate work, just return an int
// and use the same positions for players and weapons
int handle_suggest_mouse(){
	// can use SCREEN_WIDTH and SCREEN_HEIGHT if needed
	int mouse_x = 0;
	int mouse_y = 0;		// mouse coordinates
	
	int result = -1;

	SDL_GetMouseState(&mouse_x, &mouse_y);

	// get actions based on mouse location
	if ((mouse_x > accuse_x1) && (mouse_x < accuse_x2) && (mouse_y > accuse_y1) && (mouse_y < accuse_y2)){
		result = 1;
	}
	if ((mouse_x > accuse_x3) && (mouse_x < accuse_x4) && (mouse_y > accuse_y1) && (mouse_y < accuse_y2)){
		result = 2;
	}
	if ((mouse_x > accuse_x5) && (mouse_x < accuse_x6) && (mouse_y > accuse_y1) && (mouse_y < accuse_y2)){
		result = 3;
	}
	if ((mouse_x > accuse_x1) && (mouse_x < accuse_x2) && (mouse_y > accuse_y3) && (mouse_y < accuse_y4)){
		result = 4;
	}
	if ((mouse_x > accuse_x3) && (mouse_x < accuse_x4) && (mouse_y > accuse_y3) && (mouse_y < accuse_y4)){
		result = 5;
	}
	if ((mouse_x > accuse_x5) && (mouse_x < accuse_x6) && (mouse_y > accuse_y3) && (mouse_y < accuse_y4)){
		result = 6;
	}
	if ((mouse_x > accuse_x1) && (mouse_x < accuse_x2) && (mouse_y > accuse_y5) && (mouse_y < accuse_y6)){
		result = 7;
	}
	if ((mouse_x > accuse_x3) && (mouse_x < accuse_x4) && (mouse_y > accuse_y5) && (mouse_y < accuse_y6)){
		result = 8;
	}
	if ((mouse_x > accuse_x5) && (mouse_x < accuse_x6) && (mouse_y > accuse_y5) && (mouse_y < accuse_y6)){
		result = 9;
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

bool return_to_board(){
	int mouse_x = 0;
	int mouse_y = 0;		// mouse coordinates

	SDL_GetMouseState(&mouse_x, &mouse_y);

	if ((mouse_y > 5 * SCREEN_HEIGHT / 6) && (mouse_x > 2 * SCREEN_WIDTH / 3)){
		return true;
	}
	
	return false;
}


bool in_room(int location){
	// find() method doesn't work with needed compiler version
	for (int i = 0; i < sizeof(all_rooms) / sizeof(all_rooms[0]); i++){
		if (all_rooms[i] == location){
			return true;
		}
	}

	return false;
}

