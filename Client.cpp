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



// this is not perfect, but can't think of more straightforward encapsulations
void client_suggestion(SDL_Renderer* renderer, fd_set* server_set, 
	int max_connection, int client_socket, timeval quick){


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
		        		// getting response
		        		
		        	}


		        	if (!get_response){
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
							// getting suggestion response from server
							cout << "GETTING SUGGESTION RESULT FROM SERVER" << endl;
							get_weapon = false;
							get_response = true;
						}
		        	}
		        	
		        }	        
		    }
		}
		// here, we are done deciding on suggestion.  need to wait for response from server
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

				out = write(client_socket, end_turn_str.c_str(), end_turn_str.size() + 2);

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
			// get int corresponding to image_map key
			card_to_render = reverse_card_map[show_cards[i]];	
			SDL_RenderCopy(renderer, image_map[card_to_render], NULL, &suggest_rect_map[i + 1]);
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

	cout << "nested: " << card_to_show << endl;

	return card_to_show;
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

    // TODO: set player id here (pass in the message from server)
    

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
				SDL_RenderCopy(renderer, board, NULL, &background_rect);
				
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
									renderer, &server_set, max_connection, client_socket, quick
								);

								// get subsequent action
								action = ping_server(
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
		// cout << " pre-show logic: "  << action << action.substr(0, 5) << endl;
		// cout << to_string(action.substr(0, 5).compare("Show:")) << endl;

		
		if (action.substr(0, 5).compare("Show:") == 0){
    		// being forced to show a card
			card_to_show = getting_suggested(action, renderer);

			cout << "sending to server: " << card_to_show << endl;

			out = write(client_socket, card_to_show.c_str(), card_to_show.size() + 2);
		    if (out < 0){
		        cerr << "Error sending message to server, exiting..." << endl;
		        exit(1);
		    }

    	}
    	// other incoming strings we need to handle:
    	// action.compare(no_show_individual) == 0		// someone tried to suggest you, you didn't have any of the cards
    	// action.substr(0, 9).compare("No cards:") 	// someone tried to suggest someone else; they had none of the cards.
    		// action.substr(10) should have the suggested player encoded there




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




