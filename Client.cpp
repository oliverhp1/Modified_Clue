#include "Client.h"
#include "globals.h"
#include "client_helper.h"

#include <netdb.h>
#include <stdio.h> 
#include <string.h>
#include <sstream>
#include <csignal>
#include <boost/algorithm/string.hpp>


using namespace std;







// this is not perfect, but can't think of more straightforward encapsulations
void client_suggestion(SDL_Renderer* renderer, fd_set* server_set, 
	int max_connection, int client_socket, int player_id, timeval quick){


	SDL_Event e;
	string action, response, look_str;
	int suggested_player, showing_player, mouse_output, out;

	vector<string> player_and_card;


	// first get player suggestion
	bool get_player = true;
	bool get_weapon = false;
	bool get_response = false;
	bool confirming = false;

	// iterate until all suggestion logic has concluded
	while (get_player || get_weapon || get_response || confirming){
		// need to render in the inner loop as well
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, suggest_background, NULL, &background_rect);

		// render relevant cards
		if (get_player){
			for (int i = 1; i <= 6; i++){
				SDL_RenderCopy(renderer, card_image_map[i], NULL, &accuse_rect_map[i]);	
			}
		}
		else if (get_weapon){
			for (int i = 7; i <= 12; i++){
				SDL_RenderCopy(renderer, card_image_map[i], NULL, &accuse_rect_map[i - 6]);	
			}
		}
		
		// also render board highlighting here!
		// we'll need the mouse_output info, so if doing that, stick renderpresent below the poll event while loop

		// if (confirming) // render the message overlay with a continue button

		SDL_RenderPresent(renderer);


		// now poll for event
		// get mouse information
		mouse_output = handle_suggest_mouse();

		
		if (!get_response && !confirming){
			// if getting player or weapon, check for mouse click, else do nothing
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
		        		cout << "error: invalid mouse output in client_suggestion" << endl;		        		
		        	}


		        	// after handling click, send suggestion to server
				    out = write(client_socket, response.c_str(), response.size() + 2);
				    if (out < 0){
				        cerr << "Error sending message to server, exiting..." << endl;
				        exit(1);
				    }

				    // after we write to server, expect a response
				    // update action according to message from server
				    action = ping_server(
						server_set, max_connection, client_socket, quick
					);

					if (action.compare(request_weapon) == 0){
						get_player = false;
						get_weapon = true;

						// move suggested player to current location
						player_locations[mouse_output - 1] = player_locations[player_id];
					}
					else if (action.compare(invalid_input) == 0){
						// do nothing
					}
					else {
						// getting suggestion response from server
						cout << "GETTING SUGGESTION RESULT FROM SERVER" << endl;
						get_weapon = false;
						get_response = true;
					}
		        	
		        }	        
		    }
		}
		// now, we are done deciding on suggestion.  need to wait for response from server
		// that will tell us who showed what card
		else if (get_response){
    		// HERE WE GET THE OUTPUT OF OUR SUGGESTION
			// WE ALREADY KNOW THE PLAYER THAT IS GOING TO MOVE (SINCE WE SUGGESTED THEM)
			// WE NEED TO GET A SEQUENCE OF NUMBERS INDICATING WHO SHOWED WHAT CARD (or if they did not have any)

			action = ping_server(
				server_set, max_connection, client_socket, quick
			);

			if ((action.length() == 9) && (action.substr(0, 9).compare("No cards:") == 0)){
				// TODO: write a notification badge 
				cout << "player " << action.substr(10) << "has none of the suggested cards " << endl;
			}
			else if ((action.length() > 5) && (action.substr(0, 5).compare("Look:") == 0)){
				// someone is showing you a card
				// this is just a notification badge as well! no actual logic required
				look_str = action.substr(5);

				boost::split(
					player_and_card, look_str, is_semicolon
				);
				istringstream(player_and_card[0]) >> showing_player;
				cout << "player " << player_and_card[0] << " shows you " << player_and_card[1] << endl;



				get_response = false;
				confirming = true;
				// at this point, a player has responded with their card
				// we just need the player to hit confirm to continue
			}
			else if (action.compare(nobody_showed) == 0){
				// TODO: write a notification badge with continue button
				cout << "nobody had any of the cards" << endl;

				// if we get here, we are done as well; just need to hit confirm to continue
				get_response = false;
				confirming = true;
			}
    	}
    	else {
    		// just waiting to confirm
    		// TODO: this should be a button with a continue button. for now just send end_turn_str to server

    		cout << "confirming suggestion ending." << endl;

    		out = write(client_socket, end_turn_str.c_str(), end_turn_str.size() + 2);

    		confirming = false;

    	}
	}

	cout << "done." << endl;


	return;
}


// some parts similar to suggest but there are significant differences
// return true if successful, false if deactivated
bool client_accusation(SDL_Renderer* renderer, fd_set* server_set, 
	int max_connection, int client_socket, timeval quick){


	SDL_Event e;
	string action, response, case_file_str;
	int suggested_player, mouse_output, out;
	bool correct;

	vector<string> case_file;



	// first get player accusation
	bool get_player = true;
	bool get_weapon = false;
	bool get_location = false;
	bool handle_response = false;
	bool confirming = false;

	// iterate until all accusation logic has concluded
	while (get_player || get_weapon || handle_response || get_location || confirming){
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, suggest_background, NULL, &background_rect);

		// render relevant cards
		if (get_player){
			for (int i = 1; i <= 6; i++){
				SDL_RenderCopy(renderer, card_image_map[i], NULL, &accuse_rect_map[i]);	
			}
		}
		else if (get_weapon){
			for (int i = 7; i <= 12; i++){
				SDL_RenderCopy(renderer, card_image_map[i], NULL, &accuse_rect_map[i - 6]);	
			}
		}
		else if (get_location){
			for (int i = 13; i <= 21; i++){
				SDL_RenderCopy(renderer, card_image_map[i], NULL, &accuse_rect_map[i - 12]);
			}
		}
		
		// also render board highlighting here! (after getting mouse_output)
		// we'll need the mouse_output info, so if doing that, stick renderpresent below the poll event while loop

		// if (confirming) // render the message overlay with a continue button

		SDL_RenderPresent(renderer);


		// now poll for event
		// get mouse information
		mouse_output = handle_suggest_mouse();

		
		if (!handle_response && !confirming){
			// if getting player or weapon, check for mouse click, else do nothing
		    while (SDL_PollEvent(&e)){
		        if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON_LEFT)){
		        	cout << "click: " << mouse_output << endl;

		        	// get the area that was clicked on
		        	// for players and weapons, only allow up to 6
		        	// send to server all together at the end.
		        	if (mouse_output == -1){
		        		// null click; don't send anything to server
		        		continue;
		        	}
		        	else if (get_player){
		        		if (mouse_output > 6) continue; 	// invalid player
		        		response = card_map[mouse_output];
		        	}
		        	else if (get_weapon){
		        		if (mouse_output > 6) continue; 	// invalid weapon
		        		response = card_map[mouse_output + 6];
		        	}
		        	else if (get_location){
		        		response = card_map[mouse_output + 12];
		        	}
		        	else {
		        		// getting response
		        		cout << "error: incorrect logic in client_accusation" << endl;
		        	}


		        	// send whatever we decided to server
				    out = write(client_socket, response.c_str(), response.size() + 2);
				    if (out < 0){
				        cerr << "Error sending message to server, exiting..." << endl;
				        exit(1);
				    }

				    // after we write to server, expect a response
				    // update action according to message from server
				    action = ping_server(
						server_set, max_connection, client_socket, quick
					);

					if (action.compare(request_weapon) == 0){
						get_player = false;
						get_weapon = true;
					}
					else if (action.compare(accuse_location) == 0){
						get_weapon = false;
						get_location = true;
					}
					else if (action.compare(invalid_input) == 0){
						// do nothing
					}
					else {
						// getting suggestion response from server
						cout << "process the case file from server" << endl;
						get_location = false;
						handle_response = true;
					}
		        	
		        }	        
		    }
		}
		// here, we are done deciding on accusation.  need to get result from server
		else if (handle_response){
			// action = ping_server(
			// 	server_set, max_connection, client_socket, quick
			// );

			if ((action.length() >= 17) && (action.substr(0, 17).compare("Wrong accusation:") == 0)){
				// TODO: write a notification badge 
				// should we change the background too?
					// sure, this should be relatively easy
				// we should do something that persists for the rest of the game state
				case_file_str = action.substr(17);
				boost::split(
					case_file, case_file_str, is_semicolon
				);

				cout << "Wrong accusation: " << case_file[0] << ", "  
					 << case_file[1] << ", " << case_file[2] << endl;

				handle_response = false;
				confirming = true;

				correct = false;
			}
			else if ((action.length() >= 10) && (action.substr(0, 10).compare("Case file:") == 0)){
				// successful accusation
				case_file_str = action.substr(10);
				boost::split(
					case_file, case_file_str, is_semicolon
				);

				cout << "Correct: you win" << case_file[0] << ", "  
					 << case_file[1] << ", " << case_file[2] << endl;

				handle_response = false;
				confirming = true;

				correct = true;
				// at this point, we know whether we won or lost
				// we just need the player to hit confirm to continue
			}
			else {
				cout << "client_accusation- unexpected message from server: " << action << endl;
			}
    	}
    	else {
    		// just waiting to confirm
    		// TODO: this should be a button with a continue button. for now just send end_turn_str to server

    		cout << "confirming accusation ending." << endl;

    		// out = write(client_socket, end_turn_str.c_str(), end_turn_str.size() + 2);

    		confirming = false;

    	}
	}

	cout << "done." << endl;


	return correct;
}



// return index of the 3 cards to show
string getting_suggested(string action, SDL_Renderer* renderer){
	string all_card_str = action.substr(5);
	vector<string> show_cards;

	SDL_Event e;
	int n_options, card_to_render, mouse_output;
	string card_to_show;

	boost::split(
		show_cards, all_card_str, is_semicolon
	);

	n_options = show_cards.size();
	mouse_output = n_options + 10;	// set default


	for (int i = 0; i < n_options; i++){
		cout << show_cards[i] << endl;
	}


	// test rendering here, until anything is clicked
	bool done = false;
	while (!done){
		// render everything every loop
		SDL_RenderClear(renderer);		
		SDL_RenderCopy(renderer, suggest_background, NULL, &background_rect);
		for (int i = 0; i < n_options; i++){
			// render all matching cards
			// get int corresponding to card_image_map key
			card_to_render = reverse_card_map[show_cards[i]];	
			SDL_RenderCopy(renderer, card_image_map[card_to_render], NULL, &suggest_rect_map[i + 1]);
		}

		SDL_RenderPresent(renderer);	// update screen 


		// now get mouse activity
		// exit when clicked on a valid card
		while (SDL_PollEvent(&e)){
	        if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON_LEFT)){
	        	mouse_output = handle_getting_suggested_mouse();
	        }
	    }

	    if (mouse_output <= n_options){
	    	card_to_show = show_cards[mouse_output - 1];
	    	done = true;
	    }
	}

	// cout << "nested: " << card_to_show << endl;

	return card_to_show;
}




// note much of the network programming is similar to the server side code
// this also includes functionality for the GUI (using SDL)
int main(int argc, char *argv[]){
	int client_socket, client_id, address_length, player_id;
	struct hostent *server;
	char buffer[STREAM_SIZE];
	string tmp;

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

		// initialize png and jpg loading
		int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(img_flags) & img_flags)){
			printf("couldn't initialize png and jpg, error: %s\n", IMG_GetError());
		}
	}

	// load clue images
	load_all_media(renderer);



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
	
	int incoming_stream;
	bool turn_end = false;
	bool done = false;


	// SDL_RenderClear(renderer);


	
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

		// now we need to handle logic that happens when it's not your turn
		// unfortunately these pretty much all have different logic
		// so we can't aggregate all the conditions together
		// TODO: maybe stick it all in a new method

		// cout << " pre-show logic: "  << action << action.substr(0, 5) << endl;
		// cout << to_string(action.substr(0, 5).compare("Show:")) << endl;

		if (action.substr(0, 9).compare("Suggests:") == 0){
			// update location for whoever was suggested
			tmp = action.substr(9);
			boost::split(split_message, tmp, is_semicolon);

			tmp_player = reverse_card_map[split_message[1]] - 1;	// player_id is 0 indexed
			tmp_location = reverse_location_map[split_message[3]];
			player_locations[tmp_player] = tmp_location;



			// TODO: NOTIFICATION BANNER HERE
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
			// someone else is moving
			tmp = action.substr(7);
			boost::split(
				split_message, tmp, is_semicolon
			);
			istringstream(split_message[0]) >> tmp_player;
			istringstream(split_message[1]) >> tmp_location;

			player_locations[tmp_player] = tmp_location;
		}
    	// other incoming strings we need to handle: (make sure to check string length beforehand)
    	// action.compare(no_show_individual) == 0		// someone tried to suggest you, you didn't have any of the cards
    	// action.substr(0, 9).compare("No cards:") 	// someone tried to suggest someone else; they had none of the cards.
    		// action.substr(10) should have the suggested player encoded there
    	// show_card_broadcast = "Show broadcast:"	// someone else showed someone else a card

    	// for accusations:
    	// action.substr(0, 11).compare("Accusation:")	// someone tried an accusation
    	// "Case file:"		// someone successfully accused. returns 3 strings, go to end game screen and show winner/case file
    	// "Deactivated:"	// unsuccessfully accused. returns an int for player_id
    			// do clients need to track which players are deactivated? i don't think it impacts them




		// looks like we need this to maintain the gui, else it disappears
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
	        	// probably just handle the 'show hand' toggle here
	        		// actually to make it easier, only allow it on your turn for now

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




