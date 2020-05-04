#include "Client.h"
#include "globals.h"

#include <netdb.h>
#include <stdio.h> 
#include <string.h>
#include <csignal>
#include <boost/algorithm/string.hpp>

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

using namespace std;






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
		cout << "PLZZZ" << endl;
	}

	// sTexture = tmp_texture;
	return tmp_texture;

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




string ping_server(fd_set* server_set, int max_connection, int client_socket, timeval quick){
	string message = "";
	char buffer[STREAM_SIZE];
	int incoming_stream;

	// reset fd_set each time
	FD_ZERO(server_set);
	FD_SET(client_socket, server_set);
	
	// select will get any messages from server without waiting
	int incoming_action = select(
		max_connection + 1, server_set, nullptr, nullptr, &quick
	);

	// cout << to_string(incoming_action) << endl;

	if (FD_ISSET(client_socket, server_set)){
		incoming_stream = read(client_socket, buffer, STREAM_SIZE);

		if (incoming_stream == 0){
			// server_socket is no longer connected
			close(client_socket);
			FD_CLR(client_socket, server_set);
			cerr << "server disconnected, exiting." << endl;
			exit(1);
		}
		else {
			// handle the message
			// terminate char array for string handling
			buffer[incoming_stream] = '\0';
			message = buffer;

			cout << "Message received from server: " << message << endl;
			
		}
	}

	return message;
}



// unfortunate but can't think of more straightforward encapsulations
void client_suggestion(SDL_Texture* suggest_background, SDL_Renderer* renderer,
		SDL_Rect* backgroundRect, fd_set* server_set, int max_connection, 
		int client_socket, timeval quick){


	SDL_Event e;
	string action, response;
	int suggested_player, mouse_output, out;

	// first get player suggestion
	bool get_player = true;
	bool get_weapon = false;

	// iterate until suggestion has concluded
	while (get_player || get_weapon){
		// need to render in the inner loop as well
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, suggest_background, NULL, backgroundRect);
		
		// also render board highlighting here!
		// we'll need the mouse_output info, so if doing that, stick renderpresent below the poll event while loop

		SDL_RenderPresent(renderer);


		// now poll for event
		// get mouse information
		mouse_output = handle_suggest_mouse();

		// check for mouse click, else do nothing
	    while (SDL_PollEvent(&e)){
	        if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON_LEFT)){
	        	cout << "click: " << mouse_output << endl;

	        	// get the area that was clicked on
	        	// send to server all together at the end.
	        	if (mouse_output == -1){
	        		// null click; don't send anything to server
	        		continue;
	        	}
	        	else if (get_player){
	        		response = card_map[mouse_output];
	        	}
	        	else if (get_weapon){
	        		response = card_map[mouse_output + 6];
	        	}
	        	else {
	        		// should never get here; leave for debugging
	        		cerr << "error: while loop should've broken" << endl;
	        		get_player = false;
	        		get_weapon = false;
	        		break;
	        	}

	        	cout << "sending to server: " << response << endl;

	        	// send suggestion to server
			    out = write(client_socket, response.c_str(), response.size() + 2);
			    if (out < 0){
			        cerr << "Error sending message to server, exiting..." << endl;
			        exit(1);
			    }

			    // after we write to server, expect a response
			    // update action according to message from server
			    	// TODO: THIS MIGHT TAKE A WHILE IF PLAYERS NEED TO DECIDE WHAT CARD TO PICK
			    	// NESTED INNER LOOP MAKES SENSE, BUT IS A MESS
			    	// SO PROBABLY MAKE ANOTHER BOOLEAN FLAG IN THE EXISTING STRUCTURE
			    action = ping_server(
					server_set, max_connection, client_socket, quick
				);

				if (action.compare(request_weapon) == 0){
					get_player = false;
					get_weapon = true;
					suggested_player = mouse_output;
				}
				else if (action.compare(invalid_input) == 0){
					// do nothing
				}
				else {
					cout << "SUGGESTION RESULT FROM SERVER:" << endl;
					cout << action << endl;
					// HERE WE GET THE OUTPUT OF OUR SUGGESTION
					// WE ALREADY KNOW THE PLAYER THAT IS GOING TO MOVE (SINCE WE SUGGESTED THEM)
					// WE NEED TO GET A SEQUENCE OF NUMBERS INDICATING WHO SHOWED WHAT CARD (AND WHO DID NOT HAVE ANY OF THEM)
				}
	        }	        
	    }
	}
}






// note much of the network programming is similar to the server side code
// this also includes functionality for the GUI (using SDL)
int main(int argc, char *argv[]){
	int client_socket, client_id, address_length, player_id;
	struct hostent *server;
	char buffer[STREAM_SIZE];

	// each client needs this themselves
	populate_location_map();
	populate_card_map();
	populate_bridge();


	// get port and host from runtime args
	if (argc < 3) {
       printf("Usage: %s hostname port\n", argv[0]);
       exit(1);
    }
	int port = atoi(argv[2]);
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        cerr << "Undefined host, exiting...\n" << endl;
        exit(1);
    }


	// or 0 as last param
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
    	cerr << "Error creating client socket, exiting..." << endl;
		exit(1);
	}

	
	// same address as server
	struct sockaddr_in address;
	
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;	// may need to get this from the server, i.e. server->h_addr
	address_length = sizeof(address);


	// attempt connection to server socket
	client_id = connect(client_socket, (struct sockaddr*) &address, address_length);
	if (client_id < 0){
        cerr << "Unable to connect to server; exiting..." << endl;
		exit(1);
	}



	// get connection message from server
	int out;
    out = read(client_socket, buffer, 255);
    if (out < 0){
    	cerr << "Error getting message from server, exiting..." << endl;
    	exit(1);
    }

    cout << "Message from server:\n\t" << buffer << endl;

    // TODO: set player id here
    

    bool start = false;
    string message;

    // attempt to start the game 
    while (!start){
    	// clear buffer
    	memset(buffer, 0, 255);
    	fgets(buffer, 255, stdin);	// get client message from console

		// send message to server ("start" will confirm on server-side)
	    out = write(client_socket, buffer, strlen(buffer));
	    if (out < 0){
	        cerr << "Error sending message to server, exiting..." << endl;
	        exit(1);
	    }

	    // if started, open up the gui, then listen for your turn
	    if (strncmp(buffer, "start", 5) == 0){
	    	start = true;
	    }
    }



    // when game starts, load all media
    // and initialize all static SDL/GUI functionality

    // initialize sdl
    int success;
    success = SDL_Init(SDL_INIT_VIDEO);

    if (success < 0){
    	printf("Error on initialization: %s\nexiting...", SDL_GetError());
    	exit(1);
    }


    // initialize window for GUI
    SDL_Window* window = NULL;
    window = SDL_CreateWindow(
    	"CLUE!", 
    	SDL_WINDOWPOS_UNDEFINED, 
    	SDL_WINDOWPOS_UNDEFINED, 
    	SCREEN_WIDTH, 
    	SCREEN_HEIGHT, 
    	SDL_WINDOW_SHOWN
    );

    if (window == NULL){
		printf("Error on creating window: %s\nexiting...", SDL_GetError() );
		exit(1);
	}



	// initialize renderer (will draw on the window)
	SDL_Renderer* renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (renderer == NULL){
		printf("couldn't make renderer; error: %s\n", SDL_GetError() );
	}
	else{
		// set renderer draw colour
		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );	// black

		// initialize png and jpg loading
		int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(img_flags) & img_flags)){
			printf("couldn't initialize png and jpg, error: %s\n", IMG_GetError());
		}
	}

	// load clue images
	SDL_Texture* board = NULL;
	SDL_Texture* suggest_background = NULL;

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

	// create background
	SDL_Rect backgroundRect = {0, 0, SCREEN_WIDTH,SCREEN_HEIGHT};





	// we need reading from socket to be instantaneous,
	// since render is not maintainable.
	// fd_set select is able to do that for us:
	// define a quick timeout so select returns immediately
	fd_set server_set;	
	int max_connection = client_socket;
	struct timeval quick;
	quick.tv_sec = 0;
	quick.tv_usec = 100000;	// 0.1 seconds




	SDL_Event e;
	bool quit = false;
	bool premature_quit = false;	// if someone exits their window
	string action, response, mouse_output, temp_cards;
	int incoming_stream;
	bool turn_end = false;

	vector<string> show_cards;

	// SDL_RenderClear(renderer);


	
	// this is the main loop for each client
	// different functionality depending on whether it is your turn
	// we also render every iteration, since the game state changes

	while (!quit){
		// wait on any message from server
		// different messages will correspond to different logic
		// e.g. turn start, or change in game state from other player's turn

		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, board, NULL, &backgroundRect);

		SDL_RenderPresent(renderer);	// update screen 

		// cout << "plz" << endl;	// for debugging
		


		// quick ping server to see if any messages came in
		action = ping_server(
			&server_set, max_connection, client_socket, quick
		);

		// we will have different conditionals for your turn vs 
		// other actions like getting suggested 
		// on the server side, receive confirmation, before going into the type of move
		if (action.compare(start_str) == 0){
			response = "confirmed.";
			out = write(client_socket, response.c_str(), response.size() + 2);
		    if (out < 0){
		        cerr << "Error sending message to server, exiting..." << endl;
		        exit(1);
		    }

			cout << "turn starting!" << endl;

			// get initial turn action possibilities
			incoming_stream = read(client_socket, buffer, STREAM_SIZE);
			buffer[incoming_stream] = '\0';
			action = buffer;

			cout << "first action: " << action << endl;

			turn_end = false;
			// iterate until turn has concluded
			while (!turn_end){
				// server will tell us what moves are possible
				// unfortunately we can't just let the server handle everything
				
				// since we need to render highlights and whatnot
					// ALTHOUGH we can handle that later separately
					// using only the action string??

				// no, we will need to handle stuff in here
				// e.g. if you navigate, we need to know whether it's automatic or choice


				

				// get player move
			
				// need to render in the inner loop as well
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, board, NULL, &backgroundRect);
				
				// also render board highlighting here!

				SDL_RenderPresent(renderer);


				// now poll for event
				// get mouse information
				mouse_output = handle_board_mouse();

				// check for mouse click, else do nothing
			    while (SDL_PollEvent(&e)){
					if (e.type == SDL_QUIT){
			            quit = true;
			            premature_quit = true;
			        }
			        else if (!turn_end && (e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON_LEFT)){
			        	cout << "click: " << mouse_output << endl;

			        	if (mouse_output.compare(invalid_str) == 0){
			        		cout << "boundary click" << endl;
			        		turn_end = false;
			        	}
			        	else if (mouse_output.compare(navigate_str) == 0){
			        		// send to server

			        		// usually buffer has 2 characters at end for termination
			        		// so add this to the size to send the whole string
						    out = write(client_socket, mouse_output.c_str(), mouse_output.size() + 2);
						    if (out < 0){
						        cerr << "Error sending message to server, exiting..." << endl;
						        exit(1);
						    }

						    // after we write to server, expect a response
						    // update action according to message from server
						    action = ping_server(
								&server_set, max_connection, client_socket, quick
							);

							if (action.compare(force_move) == 0){
								// don't need to do anything, just render on the next iteration
								action = ping_server(
									&server_set, max_connection, client_socket, quick
								);
							}
						    
			        	}
			        	else if (mouse_output.compare(suggest_str) == 0){
			        		out = write(client_socket, mouse_output.c_str(), mouse_output.size() + 2);
						    if (out < 0){
						        cerr << "Error sending message to server, exiting..." << endl;
						        exit(1);
						    }

						    // and get response
						    action = ping_server(
								&server_set, max_connection, client_socket, quick
							);

							if (action.compare(request_player) == 0){
								// kick off suggestion method here
								client_suggestion(
									suggest_background, renderer, &backgroundRect,
									&server_set, max_connection, client_socket, quick
								);

							}

			        	}
			        	else if (mouse_output.compare(pass_str) == 0){
			        		out = write(client_socket, mouse_output.c_str(), mouse_output.size() + 2);
						    if (out < 0){
						        cerr << "Error sending message to server, exiting..." << endl;
						        exit(1);
						    }

						    // get response from server
						    action = ping_server(
								&server_set, max_connection, client_socket, quick
							);
							if (action.compare(turn_end_str) == 0) turn_end = true;
						    
			        	}
			        	// TODO: BIN ALL LOCATIONS TOGETHER
			        	else if (mouse_output.compare(hall) == 0){
			        		out = write(client_socket, mouse_output.c_str(), mouse_output.size() + 2);
						    if (out < 0){
						        cerr << "Error sending message to server, exiting..." << endl;
						        exit(1);
						    }

						    // get response from server
						    action = ping_server(
								&server_set, max_connection, client_socket, quick
							);

			        	}
			        	else if (mouse_output.compare(study) == 0){
			        		out = write(client_socket, mouse_output.c_str(), mouse_output.size() + 2);
						    if (out < 0){
						        cerr << "Error sending message to server, exiting..." << endl;
						        exit(1);
						    }

						    // and get response
						    action = ping_server(
								&server_set, max_connection, client_socket, quick
							);


			        	}
			        	
			        	else {
			        		turn_end = false;
			        		// cout << "invalid click?" << endl;

			        		cout << "empty click?" << mouse_output << endl;
			        	}
			        }	        
			    }

			    

			}	
		}	// TURN ENDS HERE

		// now we need to handle logic that happens when it's not your turn
		
		if (action.substr(0, 5).compare("Show:") == 0){
    		// being forced to show a card
    		temp_cards = action.substr(5);
    		cout << " SHOW LOGIC?? "  << temp_cards << endl;

    		boost::split(
    			show_cards, temp_cards, is_semicolon
    		);

    		for (int i = 0; i < 2; i++){
    			cout << show_cards[i] << endl;;
    		}

    		// handle being suggested in a separate method
    	}


		// looks like we need this to maintain the gui, else it disappears
		mouse_output = handle_board_mouse();

		// SDL_WaitEvent(&e);	this is more efficient, but 
		// can freeze the board state if player is inactive
		while (SDL_PollEvent(&e)){
	        if (e.type == SDL_QUIT){
	            quit = true;
	            premature_quit = true;
	        }
	        else if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON_LEFT)){
	        	turn_end = true;
	        	if (mouse_output.compare(invalid_str) == 0){
	        		cout << "boundary click" << endl;
	        		

	        	}
	        	// else if (mouse_output.compare()){
	        	// 	cout << "TOP LEFT CLICK!" << endl;
	        	// }
	        	// else if (mouse_output == 2){
	        	// 	cout << "TOP RIGHT CLICK!" << endl;
	        	// }
	        	// else if (mouse_output == 3){
	        	// 	cout << "BOTTOM LEFT CLICK!" << endl;
	        	// }
	        	// else if (mouse_output == 4){
	        	// 	cout << "BOTTOM RIGHT CLICK!" << endl;
	        	// }
	        	// else {
	        	// 	turn_end = false;
	        	// 	cout << "invalid click?" << endl;
	        	// }
	        }	        
	    }
		


		
		

	    
	    // each iteration of the main client game loop
	    // cout << "one iteration" << endl;	// for debugging
	    
	}

	

	// at this point, game has ended.

    // clear memory used by SDL
	SDL_DestroyWindow( window );
    SDL_Quit();


    // will exit immediately; need to write code for gameplay continuation
    // while (true){

    // }

    close(client_socket);
    return 0;

}




