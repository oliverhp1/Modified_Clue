#include "client_gameplay.h"
#include "client_helper.h"
#include "globals.h"



// check socket for messages from server
// return empty immediately if nothing is written
// we need this since SDL cannot maintain the images it renders
// unless we are constantly refreshing the board
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




// get desired suggestions based on where we're clicking in the gui
// and send to server
void client_suggestion(SDL_Renderer* renderer, fd_set* server_set, 
		int max_connection, int client_socket, int player_id, timeval quick){

	SDL_Event e;
	string action, response;
	int mouse_output, out;

	vector<string> player_and_card;


	// first get player suggestion
	bool get_player = true;
	bool get_weapon = false;
	bool get_response = false;
	bool confirming = false;

	// iterate until all suggestion logic has concluded
	while (get_player || get_weapon){
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

		
		// check for mouse click, else do nothing
	    while (SDL_PollEvent(&e)){
	        if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON_LEFT)){
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
				}
	        	
	        }	        
	    }
		
	}


	// done making suggestion; need to get response
	confirm_suggestion(renderer, server_set, max_connection, client_socket, quick);

	return;
}



// after making suggestion, retrieve result from server
// that will tell us who showed us what card (or if they didn't have any of the cards)
void confirm_suggestion(SDL_Renderer* renderer, fd_set* server_set, 
		int max_connection, int client_socket, timeval quick){

	SDL_Event e;
	string action, look_str;
	int showing_player, mouse_output, out;

	vector<string> player_and_card;

	bool get_response = true;
	bool confirming = false;

	// iterate until all suggestion logic has concluded
	while (get_response || confirming){
		// maintain background
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, suggest_background, NULL, &background_rect);
		SDL_RenderPresent(renderer);

		if (get_response){
    		// need to get a sequence of numbers indicating who showed us what card
    		// or if they had none of the cards
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


				// at this point, a player has responded with their card
				// we just need the player to hit confirm to continue
				get_response = false;
				confirming = true;
			}
			else if (action.compare(nobody_showed) == 0){
				// TODO: write a notification badge with continue button
				// if we really want that here, then set confirming = false so we exit immediately.

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

    return;
}





// return index of the 3 cards you want to show
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


	// rendering cards until anything is clicked
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

	return card_to_show;
}




// some parts similar to suggest but there are significant differences
// return true of accusation is correct
bool client_accusation(SDL_Renderer* renderer, fd_set* server_set, 
	int max_connection, int client_socket, timeval quick){

	SDL_Event e;
	string action, response;
	int suggested_player, mouse_output, out;


	// first get player accusation
	bool get_player = true;
	bool get_weapon = false;
	bool get_location = false;

	bool correct = false;

	// iterate until all accusation logic has concluded
	while (get_player || get_weapon || get_location){
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
				}
	        	
	        }
	    }
	}

	correct = validate_accusation(action);

	// done; return result
	return correct;
}




// given string output from server, decide if accusation was correct
bool validate_accusation(string action){
	// we are done deciding on accusation.  need to validate result from server
	string case_file_str;
	vector<string> case_file;

	bool correct = false;


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

		correct = true;
		// at this point, we know whether we won or lost
		// we just need the player to hit confirm to continue
	}
	else {
		cout << "client_accusation- unexpected message from server: " << action << endl;
	}

	return correct;
}





