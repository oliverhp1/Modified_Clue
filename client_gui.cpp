#include "client_gui.h"
#include "globals.h"

using namespace std;


// static attributes
const int SCREEN_WIDTH = 1017;
const int SCREEN_HEIGHT = 765;
const int BOARD_WIDTH = SCREEN_HEIGHT;


// coordinates for each card when getting suggested
const int suggest_y0 = SCREEN_HEIGHT / 3;
const int suggest_yf = 2 * SCREEN_HEIGHT / 3;
const int suggest1_x0 = 0;
const int suggest1_xf = 1 * BOARD_WIDTH / 3;
const int suggest2_x0 = 1 * BOARD_WIDTH / 3;
const int suggest2_xf = 2 * BOARD_WIDTH / 3;
const int suggest3_x0 = 2 * BOARD_WIDTH / 3;
const int suggest3_xf = 3 * BOARD_WIDTH / 3;

// coordinates for each card in suggest/accuse
const int accuse_y1 = SCREEN_HEIGHT / 4;
const int accuse_y2 = SCREEN_HEIGHT / 2;
const int accuse_y3 = SCREEN_HEIGHT / 2;
const int accuse_y4 = 3 * SCREEN_HEIGHT / 4;
const int accuse_y5 = 3 * SCREEN_HEIGHT / 4;
const int accuse_y6 = SCREEN_HEIGHT;

const int accuse_x1 = BOARD_WIDTH / 8;
const int accuse_x2 = 3 * BOARD_WIDTH / 8;
const int accuse_x3 = 3 * BOARD_WIDTH / 8;
const int accuse_x4 = 5 * BOARD_WIDTH / 8;
const int accuse_x5 = 5 * BOARD_WIDTH / 8;
const int accuse_x6 = 7 * BOARD_WIDTH / 8;


// coordinates for each board position
// the board is generally symmetric so a lot of quantities can be derived
const int board_start1 = 66;
const int board_start2 = 281;
const int board_start3 = 502;

const int room_width = 150;	// rooms are square
const int player_width = room_width / 3;	// players are too
const int hallway_width = 60;	// hallways are not
const int hallway_length = 75;	

const int board_end1 = board_start1 + room_width;
const int board_end2 = board_start2 + room_width;
const int board_end3 = board_start3 + room_width;

const int hall_start1 = 98;
const int hall_start2 = 320;
const int hall_start3 = 552;

const int hall_end1 = hall_start1 + hallway_width;
const int hall_end2 = hall_start2 + hallway_width;
const int hall_end3 = hall_start3 + hallway_width;

const int banner_y =  93 * SCREEN_HEIGHT / 100;

const int notif_text_y = banner_y + 8;
// const int notif_text_h = banner_y + 5;


// action buttons
const int button_width = room_width * 24 / 25;
const int button_height = hallway_length / 2 + 5;
const int button_space = 15;

const int button_x = board_end3 + 117;
const int button_y1 = board_end1 + 5;
const int button_y2 = button_y1 + button_height + button_space;
const int button_y3 = button_y2 + button_height + button_space;
const int button_y4 = button_y3 + button_height + button_space;
const int button_y5 = button_y4 + button_height + button_space;
const int button_y6 = button_y5 + button_height + button_space;


const int indicator_diameter = 95;
const int indicator_x = board_end3 + 140;
const int indicator_y = board_end1 - indicator_diameter - 12;
const int player_iconx = indicator_x;
const int player_icony = hall_end3 + 15;







// common rendering rectangles

SDL_Rect background_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
SDL_Rect board_rect = {0, 0, BOARD_WIDTH, SCREEN_HEIGHT};
SDL_Rect banner_rect = {
	3 * BOARD_WIDTH / 100, banner_y, 94 * BOARD_WIDTH / 100, 6 * SCREEN_HEIGHT / 100
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

SDL_Rect icon_rect = {player_iconx, player_icony, indicator_diameter, indicator_diameter};
SDL_Rect indicator_rect = {indicator_x, indicator_y, indicator_diameter, indicator_diameter};
SDL_Rect button1_rect = {button_x, button_y1, button_width, button_height};
SDL_Rect button2_rect = {button_x, button_y2, button_width, button_height};
SDL_Rect button3_rect = {button_x, button_y3, button_width, button_height};
SDL_Rect button4_rect = {button_x, button_y4, button_width, button_height};
SDL_Rect button5_rect = {button_x, button_y5, button_width, button_height};
SDL_Rect button6_rect = {button_x, button_y6, button_width, button_height};

SDL_Rect case_file_rect1 = {0, SCREEN_HEIGHT * 3 / 4, BOARD_WIDTH / 4, BOARD_WIDTH / 4};
SDL_Rect case_file_rect2 = {BOARD_WIDTH * 1 / 4, SCREEN_HEIGHT * 3 / 4, BOARD_WIDTH / 4, BOARD_WIDTH / 4};
SDL_Rect case_file_rect3 = {BOARD_WIDTH * 2 / 4, SCREEN_HEIGHT * 3 / 4, BOARD_WIDTH / 4, BOARD_WIDTH / 4};


SDL_Rect you_are_rect;
SDL_Rect get_player_rect;
SDL_Rect get_weapon_rect;
SDL_Rect get_location_rect;
SDL_Rect choose_card_rect;
SDL_Rect waiting_rect;

SDL_Rect final_rect;


// store positions in maps for easy access
unordered_map<int, SDL_Rect> suggest_rect_map;	// for getting suggested
unordered_map<int, SDL_Rect> accuse_rect_map;	// for suggesting and accusing
unordered_map<int, SDL_Rect> button_rect_map;	// for action buttons
unordered_map< int, vector<int> > player_render_map;	// for rendering players


// textures for rendering
SDL_Texture* board = NULL;
SDL_Texture* suggest_background = NULL;
SDL_Texture* notify_background = NULL;
SDL_Texture* hand_background = NULL;

SDL_Texture* turn_active = NULL;
SDL_Texture* lost_indicator = NULL;

SDL_Texture* you_are_text = NULL;
SDL_Texture* get_player_text = NULL;
SDL_Texture* get_weapon_text = NULL;
SDL_Texture* get_location_text = NULL;
SDL_Texture* choose_card_text = NULL;
SDL_Texture* waiting_text = NULL;

SDL_Texture* staging_screen = NULL;
SDL_Texture* final_screen = NULL;
SDL_Texture* final_win = NULL;
SDL_Texture* final_lose = NULL;

bool active_buttons[] = {false, false, false, false, false, false};

TTF_Font* dark_font = NULL;
TTF_Font* blood_font = NULL;
TTF_Font* large_blood_font = NULL;

SDL_Color grey = {145, 145, 145};	// grey
SDL_Color red = {139, 0, 0};		// dark red



// all cards' textures stored in here
unordered_map<int, SDL_Texture*> card_image_map;
unordered_map<int, SDL_Texture*> player_icon_map;
unordered_map<int, SDL_Texture*> active_button_map;
unordered_map<int, SDL_Texture*> inactive_button_map;


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




// this will render all players
// given a map of locations
// assumption: only one player per hallway
// 	and up to 6 in a room
void render_all_players(unordered_map<int, int> player_locations,
		SDL_Renderer* renderer){
	// just do the bottom two thirds of the room
	// it's a bit low, so up it by a few pixels each
	// (0, 1/3), (1/3, 1/3), (2/3, 1/3)
	// (0, 2/3), (1/3, 2/3), (2/3, 2/3)
	int tmp_x, tmp_y, tmp_location;


	for (int id = 0; id < 6; id++){
		tmp_location = player_locations[id];

		tmp_x = player_render_map[tmp_location][0];
		tmp_y = player_render_map[tmp_location][1];

		if (in_room(tmp_location)){
			// players 0-2 are in the top row 
			tmp_x += (id % 3) * room_width / 3;
			tmp_y += room_width / 3 - 10;

			// players 3-5 are in the bottom row
			if (id >= 3){
				tmp_y += room_width / 3;
			}
		}

		SDL_Rect temp_rect = {tmp_x, tmp_y, player_width, player_width};

		SDL_RenderCopy(renderer, player_icon_map[id + 1], NULL, &temp_rect);
	}

	// ** these are all relative to the top left corner of the room
	// for hallways, stick directly in the middle
}



void update_active_buttons(string action){
	if (action.compare(navigate_stay_str) == 0){
		bool temp[] = {true, false, true, false, false, true};
		copy(temp, temp + 6, active_buttons);
	}
	else if (action.compare(request_location) == 0){
		bool temp[] = {false, false, false, false, false, false};
		copy(temp, temp + 6, active_buttons);
	}
	else if (action.compare(suggest_accuse_str) == 0){
		bool temp[] = {false, true, false, false, true, true};
		copy(temp, temp + 6, active_buttons);
	}
	else if (action.compare(accuse_pass_str) == 0){
		bool temp[] = {false, false, false, true, true, true};
		copy(temp, temp + 6, active_buttons);
	}
	else if (action.compare(navigate_accuse_str) == 0){
		bool temp[] = {true, false, false, false, true, true};
		copy(temp, temp + 6, active_buttons);
	}
	else if (action.compare("deactivate") == 0){
		bool temp[] = {false, false, false, false, false, false};
		copy(temp, temp + 6, active_buttons);
	}

	// otherwise, make sure not to change it
	return;
}
 	



// 6 buttons, 6 boolean flags for which version to use
void render_all_buttons(SDL_Renderer* renderer){

	for (int i = 0; i < 6; i++){
		if (active_buttons[i]){
			SDL_RenderCopy(renderer, active_button_map[i+1], NULL, &button_rect_map[i+1]);
		}
		else {
			SDL_RenderCopy(renderer, inactive_button_map[i+1], NULL, &button_rect_map[i+1]);
		}
	}

	return;
}




// this will add notifications to the queue (based on notification_text)
void push_banner(string notification_text, SDL_Renderer* renderer, 
		queue< SDL_Texture* > *pending_notifications, queue<SDL_Rect> *pending_rect){

	SDL_Surface* tmp_surface = NULL;
	tmp_surface = TTF_RenderText_Solid(blood_font, notification_text.c_str(), red);

	SDL_Rect notif_rect = {
		BOARD_WIDTH / 2 - (tmp_surface->w) / 2, notif_text_y, tmp_surface->w, tmp_surface->h
	};

	pending_notifications->push(SDL_CreateTextureFromSurface(renderer, tmp_surface));
	pending_rect->push(notif_rect);
	SDL_FreeSurface(tmp_surface);
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
void load_all_media(SDL_Renderer* renderer, int player_id){
    board = load_image("images/board.png", renderer);
	if (board == NULL){
		cerr << "Couldn't load board... exiting\n" << endl;
		exit(1);
	}

	// LOAD ALL OTHER IMAGES HERE 
	suggest_background = load_image("images/suggest.jpg", renderer);
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

	hand_background = load_image("images/hand.jpg", renderer);
	if (hand_background == NULL){
		cerr << "Couldn't load hand image... exiting\n" << endl;
		exit(1);
	}

	staging_screen = load_image("images/staging.png", renderer);
	if (staging_screen == NULL){
		cerr << "Couldn't load staging screen image... exiting\n" << endl;
		exit(1);
	}

	final_screen = load_image("images/end_game.png", renderer);
	if (final_screen == NULL){
		cerr << "Couldn't load final screen image... exiting\n" << endl;
		exit(1);
	}



	// load all cards into map
	for (int i = 1; i <= 21; i++){
		SDL_Texture* temp_texture = NULL;
		temp_texture = load_image("images/cards/" + to_string(i) + ".png", renderer);

		card_image_map[i] = temp_texture;
	}

	// load all icons into map
	for (int i = 1; i <= 6; i++){
		SDL_Texture* temp_texture = NULL;
		temp_texture = load_image("images/icons/" + to_string(i) + "_icon.png", renderer);

		player_icon_map[i] = temp_texture;
	}


	// different fonts
	dark_font = TTF_OpenFont("fonts/a_dark_wedding.ttf", 4 * SCREEN_HEIGHT / 100);
	if (dark_font == NULL){
		printf("couldn't load dark wedding, error: %s\n", TTF_GetError());
		exit(1);
	}

	blood_font = TTF_OpenFont("fonts/bloody.ttf", 3 * SCREEN_HEIGHT / 100);
	if (blood_font == NULL){
		printf("couldn't load bloody font, error: %s\n", TTF_GetError());
		exit(1);
	}

	large_blood_font = TTF_OpenFont("fonts/bloody.ttf", 8 * SCREEN_HEIGHT / 100);
	if (large_blood_font == NULL){
		printf("couldn't load bloody font, error: %s\n", TTF_GetError());
		exit(1);
	}


	// all buttons and indicators
	turn_active = load_image("images/buttons/your_turn.png", renderer);
	lost_indicator = load_image("images/buttons/lost.png", renderer);
	
	active_button_map[1] = load_image("images/buttons/navigate_active.png", renderer);
	active_button_map[2] = load_image("images/buttons/suggest_active.png", renderer);
	active_button_map[3] = load_image("images/buttons/stay_active.png", renderer);
	active_button_map[4] = load_image("images/buttons/pass_active.png", renderer);
	active_button_map[5] = load_image("images/buttons/accuse_active.png", renderer);
	active_button_map[6] = load_image("images/buttons/show_hand_active.png", renderer);

	inactive_button_map[1] = load_image("images/buttons/navigate_inactive.png", renderer);
	inactive_button_map[2] = load_image("images/buttons/suggest_inactive.png", renderer);
	inactive_button_map[3] = load_image("images/buttons/stay_inactive.png", renderer);
	inactive_button_map[4] = load_image("images/buttons/pass_inactive.png", renderer);
	inactive_button_map[5] = load_image("images/buttons/accuse_inactive.png", renderer);
	inactive_button_map[6] = load_image("images/buttons/show_hand_inactive.png", renderer);

	for (int i = 1; i <= 6; i++){	// validate all together
		if (active_button_map[i] == NULL){
			cerr << "Couldn't load active button image... exiting\n" << endl;
			exit(1);
		}
		if (inactive_button_map[i] == NULL){
			cerr << "Couldn't load inactive button image... exiting\n" << endl;
			exit(1);
		}
	}


	string temp;

	// all text handling
	SDL_Surface* tmp_surface = NULL;
	temp = "You are " + card_map[player_id + 1];
	tmp_surface = TTF_RenderText_Solid(dark_font, temp.c_str(), grey);
	you_are_rect = (SDL_Rect) {834 - (tmp_surface->w) / 2, 570, tmp_surface->w, tmp_surface->h};
	you_are_text = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);

	tmp_surface = NULL;
	temp = request_player;
	tmp_surface = TTF_RenderText_Solid(large_blood_font, temp.c_str(), red);
	get_player_rect = (SDL_Rect) {board_start1 + 7, board_start1 + 7, tmp_surface->w, tmp_surface->h};
	get_player_text = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);

	tmp_surface = NULL;
	temp = request_weapon;
	tmp_surface = TTF_RenderText_Solid(large_blood_font, temp.c_str(), red);
	get_weapon_rect = (SDL_Rect) {board_start1 + 7, board_start1 + 7, tmp_surface->w, tmp_surface->h};
	get_weapon_text = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);

	tmp_surface = NULL;
	temp = accuse_location;
	tmp_surface = TTF_RenderText_Solid(large_blood_font, temp.c_str(), red);
	get_location_rect = (SDL_Rect) {board_start1 + 7, board_start1 + 7, tmp_surface->w, tmp_surface->h};
	get_location_text = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);

	tmp_surface = NULL;
	temp = "Choose a card to show:";
	tmp_surface = TTF_RenderText_Solid(large_blood_font, temp.c_str(), red);
	choose_card_rect = (SDL_Rect) {board_start1 + 7, board_start1 + 7, tmp_surface->w, tmp_surface->h};
	choose_card_text = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);

	tmp_surface = NULL;
	temp = "Waiting for response...";
	tmp_surface = TTF_RenderText_Solid(large_blood_font, temp.c_str(), red);
	waiting_rect = (SDL_Rect) {board_start1 + 7, board_start1 + 7, tmp_surface->w, tmp_surface->h};
	waiting_text = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);

	tmp_surface = NULL;
	temp = "You win!";
	tmp_surface = TTF_RenderText_Solid(large_blood_font, temp.c_str(), red);
	final_rect = (SDL_Rect) {board_start1 + 7, board_start1 + 7, tmp_surface->w, tmp_surface->h};
	final_win = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);

	tmp_surface = NULL;
	temp = "You lose";
	tmp_surface = TTF_RenderText_Solid(large_blood_font, temp.c_str(), red);
	final_lose = SDL_CreateTextureFromSurface(renderer, tmp_surface);
	SDL_FreeSurface(tmp_surface);



	
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

	// where to render buttons
	button_rect_map[1] = button1_rect;
	button_rect_map[2] = button2_rect;
	button_rect_map[3] = button3_rect;
	button_rect_map[4] = button4_rect;
	button_rect_map[5] = button5_rect;
	button_rect_map[6] = button6_rect;

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
	player_render_map[-1].push_back(board_start1 - player_width);
	player_render_map[-2].push_back(board_start1 - player_width);
	player_render_map[-2].push_back(board_end1 + (hallway_width / 2) - (player_width / 2));
	player_render_map[-3].push_back(board_start1 - player_width);
	player_render_map[-3].push_back(board_end2 + (hallway_width / 2) - (player_width / 2));
	player_render_map[-4].push_back(board_end1 + (hallway_width / 2) - (player_width / 2));
	player_render_map[-4].push_back(board_end3);
	player_render_map[-5].push_back(board_end2 + (hallway_length / 2) - (player_width / 2));
	player_render_map[-5].push_back(board_end3);
	player_render_map[-6].push_back(board_end3);
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
	// can use BOARD_WIDTH and SCREEN_HEIGHT if needed
	int mouse_x = 0;
	int mouse_y = 0;		// mouse coordinates
	
	int result = 36;	// return this if player clicked on nothing

	SDL_GetMouseState(&mouse_x, &mouse_y);

	// cout << "x, y = " << mouse_x << ", " << mouse_y << endl;	// for debugging purposes

	// get actions
	if ((mouse_x > button1_rect.x) && (mouse_x < button1_rect.x + button1_rect.w) 
			&& (mouse_y > button1_rect.y) && (mouse_y < button1_rect.y + button1_rect.h)){
		result = 30;
	}
	else if ((mouse_x > button2_rect.x) && (mouse_x < button2_rect.x + button2_rect.w) 
			&& (mouse_y > button2_rect.y) && (mouse_y < button2_rect.y + button2_rect.h)){
		result = 31;
	}
	else if ((mouse_x > button3_rect.x) && (mouse_x < button3_rect.x + button3_rect.w) 
			&& (mouse_y > button3_rect.y) && (mouse_y < button3_rect.y + button3_rect.h)){
		result = 32;
	}
	else if ((mouse_x > button4_rect.x) && (mouse_x < button4_rect.x + button4_rect.w) 
			&& (mouse_y > button4_rect.y) && (mouse_y < button4_rect.y + button4_rect.h)){
		result = 33;
	}
	else if ((mouse_x > button5_rect.x) && (mouse_x < button5_rect.x + button5_rect.w) 
			&& (mouse_y > button5_rect.y) && (mouse_y < button5_rect.y + button5_rect.h)){
		result = 34;
	}
	else if ((mouse_x > button6_rect.x) && (mouse_x < button6_rect.x + button6_rect.w) 
			&& (mouse_y > button6_rect.y) && (mouse_y < button6_rect.y + button6_rect.h)){
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
	// cout << "(x, y) = (" + to_string(mouse_x) + ", " + to_string(mouse_y) + ")" << endl;


	return result;
}



// this one handles the suggestion and accuse screens
// to avoid duplicate work, just return an int
// and use the same positions for players and weapons
int handle_suggest_mouse(){
	// can use BOARD_WIDTH and SCREEN_HEIGHT if needed
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

	if ((mouse_x > 678) && (mouse_x < 944) && (mouse_y > 279) && (mouse_y < 509)){
		return true;
	}

	// cout << "(x, y) = (" + to_string(mouse_x) + ", " + to_string(mouse_y) + ")" << endl;
	
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

