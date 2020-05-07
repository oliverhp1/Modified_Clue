#include "Client.h"
#include "globals.h"
#include "client_helper.h"
#include "client_gameplay.h"

#include <netdb.h>
#include <stdio.h> 
#include <string.h>
#include <sstream>
#include <csignal>
#include <boost/algorithm/string.hpp>


using namespace std;






// note much of the network programming is similar to the server side code
// this also includes functionality for the GUI (using SDL)
int main(int argc, char *argv[]){
	// client instance variables
	int client_socket, client_id, address_length, player_id;
	struct hostent *server;
	vector<int> hand;

	char buffer[STREAM_SIZE];
	string tmp;
	int tmp_card;


	// for handling delimited incoming messages from server
	int tmp_player, tmp_location, tmp_weapon;
	vector<string> split_message;



	// initialize player locations to starting blocks
	for (int i = 0; i < 6; i++){
		player_locations[i] = -1 * (i + 1);
	}

	// each client needs this themselves
	populate_location_map();
	populate_card_map();
	populate_bridge();
	fill_location_map();


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


    // set player_id based on message from server
    tmp = buffer[37];
    istringstream(tmp) >> player_id;
    

    bool start = false;
    string message;


    // attempt to start the game 
    while (!start){
    	// clear buffer
    	memset(buffer, 0, 255);
    	fgets(buffer, 255, stdin);	// get client message from console

		// send message to server ("start" will confirm on server-side)
	    // if started, open up the gui, then listen for your turn
	    if (strncmp(buffer, "start", 5) == 0){
	    	start = true;
	    	out = write(client_socket, buffer, 5);
		    if (out < 0){
		        cerr << "Error sending message to server, exiting..." << endl;
		        exit(1);
		    }
	    }
	    else {
	    	cout << "\tPlz enter start:" << endl;
	    }
    }


    // when game starts, initialize all static SDL/GUI functionality
    // and load all media/images

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

		// initialize png/jpg loading, and font handling
		int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(img_flags) & img_flags)){
			printf("couldn't initialize png and jpg, error: %s\n", IMG_GetError());
		}
		if (TTF_Init() == -1){
			printf("couldn't initialize ttf, error: %s\n", TTF_GetError() );
			success = false;
		}
	}

	// load clue images
	load_all_media(renderer);

	// prepare to render notifications
	queue< SDL_Texture* > pending_notifications;
	queue<SDL_Rect> pending_rect;

	SDL_Texture* current_notif = NULL;
	SDL_Rect* current_notif_rect = NULL;
	int notif_iteration = 0;	// todo: use time rather than iterations



	// we need reading from socket to be instantaneous,
	// since render is not maintainable.
	// fd_set select is able to do that for us:
	// define a quick timeout so select returns immediately
	fd_set server_set;	
	int max_connection = client_socket;
	struct timeval quick;
	quick.tv_sec = 0;
	quick.tv_usec = 50000;	// 0.05 seconds




	SDL_Event e;
	bool quit = false;
	bool premature_quit = false;	// if someone exits their window
	string action, response, mouse_output, temp_cards, card_to_show;
	string notification_text;
	
	int action_player, incoming_stream;
	bool turn_end = false;
	bool done = false;


	// need to get our hand
	while (!done){
		action = ping_server(
			&server_set, max_connection, client_socket, quick
		);

		if ((action.length() > 10) && (action.substr(0, 10).compare("Your hand:") == 0)){
			tmp = action.substr(10);
			boost::split(split_message, tmp, is_semicolon);
			
			for (int i = 0; i < split_message.size(); i++){
				istringstream(split_message[i]) >> tmp_card;
				cout << "testing: card " << split_message[i] << endl;
				hand.push_back(tmp_card);
			}

			done = true;
		}
	}



	done = false;
	// this is the main loop for each client
	// different functionality depending on whether it is your turn
	// we also render every iteration, since the game state changes

	while (!quit){
		// wait on any message from server
		// different messages will correspond to different logic
		// e.g. turn start, or change in game state from other player's turn

		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, board, NULL, &background_rect);
		render_all_players(player_locations, renderer);

		// 	render_notifications()		// TODO: abstract this. it's a bit tricky since we have the queue defined locally
		if (current_notif == NULL){
			if (!pending_notifications.empty()){
				cout << "getting from queue" << endl;
				current_notif = pending_notifications.front();
				current_notif_rect = &pending_rect.front();
				notif_iteration = 1;

				cout << "popping from queue" << endl;
				cout << to_string(current_notif == NULL) << endl;

				pending_notifications.pop();
				pending_rect.pop();

				cout << "empty queue? " << to_string(pending_notifications.empty()) << endl;
			}
		}
		else if (notif_iteration >= 100){
			// stop showing current notification after 10 iterations (todo later: use time)
			// on the next iteration, we will reassign current notification
			current_notif = NULL;
			current_notif_rect = NULL;
			notif_iteration = 0;
		}
		else {
			// first render background, then render current notification
			SDL_RenderCopy(renderer, notify_background, NULL, &notification_rect);
			SDL_RenderCopy(renderer, current_notif, NULL, current_notif_rect);
			notif_iteration++;
		}

		
		SDL_RenderPresent(renderer);	// update screen 
		


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
				// so we will just attempt to send all actions to the server
				// and if it returns invalid, we will keep sending outputs
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, board, NULL, &background_rect);
				render_all_players(player_locations, renderer);
				
				// also render board highlighting here!
					// use output of mouse_output. and stick renderpresent at the end of the while loop

				SDL_RenderPresent(renderer);


				// now poll for event
				// get mouse information
				mouse_output = location_map[handle_board_mouse()];

				// check for mouse click, else do nothing
				// if player clicked on an action or area, just send it to the server
				// server will determine whether it was a valid action
				// several clicks are special cases, in particular 'suggest' and 'accuse' 
				// will call helper methods
			    while (SDL_PollEvent(&e)){
					if (e.type == SDL_QUIT){
			            quit = true;
			            premature_quit = true;
			        }
			        else if (!turn_end && (e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON_LEFT)){
			        	cout << "click: " << mouse_output << endl;

			        	if (mouse_output.compare(empty_space) == 0){
			        		cout << mouse_output << endl;
			        	}
			        	else if (mouse_output.compare(show_hand_str) == 0){
			        		cout << "testing: render hand" << endl;
			        		render_hand(renderer, hand);
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
						    // if "moving", it was forced. if "request_location", handle next iteration
						    action = ping_server(
								&server_set, max_connection, client_socket, quick
							);

							if (action.substr(0, 7).compare("Moving:") == 0){
								// we are moving
								tmp = action.substr(7);
								boost::split(
									split_message, tmp, is_semicolon
								);
								istringstream(split_message[1]) >> tmp_location;

								player_locations[player_id] = tmp_location;

								// need to read additional message to clear stream
								// if (usleep(20000) == -1) cout << "failed to pause/clear buffer";
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
									renderer, &server_set, max_connection, client_socket, player_id, quick
								);

								// get subsequent action
								action = ping_server(
									&server_set, max_connection, client_socket, quick
								);

							}
			        	}
			        	else if (mouse_output.compare(accuse_str) == 0){
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
								client_accusation(
									renderer, &server_set, max_connection, client_socket, quick
								);

								// get subsequent action
								action = ping_server(
									&server_set, max_connection, client_socket, quick
								);

								// todo: if this says "Game over", print the victor and quit

							}
			        	}
			        	else if ((mouse_output.compare(pass_str) == 0) || (mouse_output.compare(stay_str) == 0)){
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

			        	else {	// BIN ALL LOCATIONS TOGETHER
			        		// got a location; only move if we get the correct message back from server
			        		out = write(client_socket, mouse_output.c_str(), mouse_output.size() + 2);
						    if (out < 0){
						        cerr << "Error sending message to server, exiting..." << endl;
						        exit(1);
						    }

						    // and get response
						    action = ping_server(
								&server_set, max_connection, client_socket, quick
							);

						    if (action.substr(0, 7).compare("Moving:") == 0){
								// we are moving
								tmp = action.substr(7);
								boost::split(
									split_message, tmp, is_semicolon
								);
								istringstream(split_message[1]) >> tmp_location;

								player_locations[player_id] = tmp_location;

								// need to read additional message to clear stream
								action = ping_server(
									&server_set, max_connection, client_socket, quick
								);
							}


			        	}
			        }	        
			    }
			}

			// each turn ends when we get here
			cout << "turn ending" << endl;
		}	

		// here, we need to handle logic that happens when it's not your turn
		// these pretty much all have different logic
		// so we can't aggregate all the conditions together

		if (action.substr(0, 9).compare("Suggests:") == 0){
			// update location for whoever was suggested
			tmp = action.substr(9);
			boost::split(split_message, tmp, is_semicolon);

			tmp_player = reverse_card_map[split_message[1]] - 1;	// player_id is 0 indexed
			tmp_location = reverse_location_map[split_message[3]];
			player_locations[tmp_player] = tmp_location;


			// and make notification banner
			istringstream(split_message[0]) >> action_player;

			notification_text = card_map[action_player + 1] 
				+ " suggested: " + split_message[1] + ", "
				+ split_message[2] + ", " + split_message[3];


			SDL_Surface* tmp_surface = NULL;
			tmp_surface = TTF_RenderText_Solid(blood_font, notification_text.c_str(), red);

			SDL_Rect notif_rect = {
				SCREEN_WIDTH / 2 - (tmp_surface->w) / 2, 92 * SCREEN_HEIGHT / 100, tmp_surface->w, tmp_surface->h
			};

			pending_notifications.push(SDL_CreateTextureFromSurface(renderer, tmp_surface));
			pending_rect.push(notif_rect);

			SDL_FreeSurface(tmp_surface);


		}

		else if (action.substr(0, 5).compare("Show:") == 0){
    		// being forced to show a card; iterate until we get a valid option
    		done = false;

    		while (!done){
    			card_to_show = getting_suggested(action, renderer);

				cout << "card to show; sending to server: " << card_to_show << endl;

				out = write(client_socket, card_to_show.c_str(), card_to_show.size() + 2);
			    if (out < 0){
			        cerr << "Error sending message to server, exiting..." << endl;
			        exit(1);
			    }

			    response = ping_server(
					&server_set, max_connection, client_socket, quick
				);

				if (response.compare(invalid_input) != 0) done = true;
    		}
		}
		else if (action.substr(0, 7).compare("Moving:") == 0){
			// someone else is moving. no need for notification, just update position
			tmp = action.substr(7);
			boost::split(
				split_message, tmp, is_semicolon
			);
			istringstream(split_message[0]) >> tmp_player;
			istringstream(split_message[1]) >> tmp_location;

			player_locations[tmp_player] = tmp_location;
		}


    	// TODO: other incoming strings we need to handle: (make sure to check string length beforehand)
    	// action.compare(no_show_individual) == 0		// someone tried to suggest you, you didn't have any of the cards
    	// action.substr(0, 9).compare("No cards:") 	// someone tried to suggest someone else; they had none of the cards.
    		// action.substr(10) should have the suggested player encoded there
    	// show_card_broadcast = "Show broadcast:"	// someone else showed someone else a card

    	// for accusations:
    	// action.substr(0, 11).compare("Accusation:")	// someone tried an accusation
    	// "Case file:"		// someone successfully accused. returns 3 strings, go to end game screen and show winner/case file
    	// "Deactivated:"	// unsuccessfully accused. returns an int for player_id
    			// do clients need to track which players are deactivated? i don't think it impacts them




		// we need this to maintain the gui, else it disappears
		mouse_output = location_map[handle_board_mouse()];

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
	        }	        
	    }
		


		
		

	    
	    // each iteration of the main client game loop
	    // cout << "one iteration" << endl;	// for debugging
	    
	}

	

	// at this point, game has ended.

    // clear memory used by SDL
	SDL_DestroyWindow( window );
    SDL_Quit();


    // will exit immediately; need to write code for gameplay continuation if we wanna render 
    // a "game over" screen or sth like that

    // while (true){

    // }

    close(client_socket);
    return 0;

}




