#include "GamePlay.h"

using namespace std;



/** These are all the actions that a player might take on their turn.
 * They are implemented as static methods so we don't have to worry about
 * keeping track of a separate object instance and calling from it.
 * The overall driver is the method 'execute_turn'.
 * Other methods correspond to general Clue gameplay:
 *	navigate
 *	suggest
 *	accuse
 *	pass
 */



/* 
	this method is the overall driver of a player's turn.
	it will call static methods from GamePlay depending on what action
	the player chooses to take.
	messages will also be pushed out to all other players.


 on a player's turn they have several options:
 	0. look at their hand (for simplicity, just show once for now)
	1. navigate 
		- from hallway to room (must do this if you start in a hallway)
		- from room to hallway (if not occupied)
			- *you must move from the room, unless you were moved there by a suggestion
		- take secret passageway (if available)
	2. make suggestion
		- you can suggest before or after navigating,
			as long as you are in a room.
		    but you cannot navigate after suggesting.
		- when you make a suggestion, the suggested player (and weapon) is moved to your room
		- then, subsequent player must show you a card you suggested
			- if they have none, it goes to the next player
			- if nobody has any, suggestion use case ends.
			- suggestion use case also ends as soon as first card is shown.
	3. make accusation
		- you can accuse at any time.
		- immediately after an accusation, you either win, or are out of the game permanently
			- so that of course means you cannot navigate or suggest afterwards.
*/

void GamePlay::execute_turn(int player_id, Server server, Player* players){
	int* socket_tracker = server.get_socket_tracker();
	int socket_id = socket_tracker[player_id];
	int n_clients = server.get_n_clients();
	Player* player = &players[player_id];
	string yn;


	
	// temp placeholder; build together with navigation logic later
	bool any_valid_moves = true;
	

	send(socket_id, start_str.c_str(), start_str.size(), 0);


	// check hand and locations here
	yn = get_valid_input(socket_id, server, check_hand, yes_no);
	if (yn == "y") show_hand(socket_id, player);

	yn = get_valid_input(socket_id, server, check_state, yes_no);
	if (yn == "y") print_state(socket_id, n_clients, players);

	


	string move;


	// there's 4 possibilities: 2x2 of (any valid moves) x (just got suggested)
	// 3 of them (all except (no valid moves) x (not just suggested))
	// will have the same suggest/accuse/pass at the end, so 
	// to reduce duplicated code, stick them together
	// the final block (no valid moves) x (not just suggested)
	// goes directly to accuse/pass

	if (any_valid_moves || player->get_just_suggested()){
		if (!player->get_just_suggested()){	
			// force them to navigate
			send(socket_id, force_navigate_str.c_str(), 
				 force_navigate_str.size(), 0);

			navigate(player_id, socket_tracker, server);
		}
		else if (any_valid_moves){
			// this implies they were just suggested
			// so they're allowed to stay if desired
			move = get_valid_input(socket_id, server, 
				navigate_stay_str, navigate_stay);

			// either navigate or stay
			if (move.compare("navigate")){
				navigate(player_id, socket_tracker, server);
			}
		}
		else {
			// just suggested and no valid moves
			send(socket_id, force_stay_str.c_str(), 
				 force_stay_str.size(), 0);
		}


		// navigation is finished at this point (if possible)
		// now they can suggest/pass/accuse
		if (in_room(player)){
			// suggest, accuse, or pass logic
			move = get_valid_input(socket_id, server, 
				suggest_accuse_str, suggest_accuse);
			call_valid_move(player_id, socket_tracker, server, move, players);

			
			// then, if suggested, accuse or pass options again
			if (move.compare("suggest") == 0){
				move = get_valid_input(socket_id, server, 
					accuse_pass_str, accuse_pass);

				call_valid_move(player_id, socket_tracker, server, move, players);
			}
		}
		else {
			// directly accuse or pass if not in room
			move = get_valid_input(socket_id, server, 
				accuse_pass_str, accuse_pass);

			call_valid_move(player_id, socket_tracker, server, move, players);
			
		}

	}
	else {	// no valid moves AND not just suggested
		send(socket_id, force_stay_str.c_str(), 
			 force_stay_str.size(), 0);	

		// directly accuse or pass
		move = get_valid_input(socket_id, server, 
			accuse_pass_str, accuse_pass);

		call_valid_move(player_id, socket_tracker, server, move, players);
	}

	
	// set just_suggested attribute to false at end of turn
	player->set_just_suggested(false);

	// and that's it.
	return ;
}




void GamePlay::navigate(int player_id, int* socket_tracker, Server server){
	int socket_id = socket_tracker[player_id];
	int n_clients = server.get_n_clients();


	// TODO: FIGURE OUT VALID CHOICES TO MOVE TO BEFOREHAND. 
	// please do this in the execute_turn() method if possible.
	// you can get all player positions using the "players" attribute.
	// for the logic, refer to the diagram and rules in the file describing our class project.
	// you can use location_map.  store a player's current location in an attribute, 
	// e.g. player::location (preferably, make it a private attribute and use setter and getter methods),
	// and then use that and the location_map to determine valid places to move.
	// then stick it in "request_location" string below.
	// syntax for location map is just: location_map[1] will return "Study", and so on. (see bottom of this file)
	// 
	// e.g. "Would you like to move to the " + option1 + " or"... etc

	// send message to client asking where they'd like to move
	send(socket_id, request_location.c_str(), request_location.size(), 0);

	// get string representing where player wants to move
	// location_buffer will store the input the player enters in the console
	string location_buffer = server.receive_communication(socket_id);


	/* TODO: 
		first check that the choice was valid.
		maybe can reuse the valid choices you found from earlier (e.g. input them as parameters into this method)
		then, actually move the player there.
		do this by updating the player attribute for location
	*/


	// communicate to all other clients
	string move_broadcast = "Player " + to_string(player_id) 
		+ " is moving to the " + location_buffer + "\n";

	for (int i = 0; i < n_clients; i++){
		if (i != player_id){
			send(socket_tracker[i], move_broadcast.c_str(), move_broadcast.size(), 0);
		}
	}

	// also output on the server
	cout << move_broadcast;
	return;

}



/** when a player makes a suggestion:
 *	we retrieve their location, as well as their guesses for character/weapon
 *	we move the suggested character to the room
 *  loop over all active players until one of them has one of the cards
 *  player must then show that card (or decide which one if multiple match)
 */
int GamePlay::suggest(int player_id, int* socket_tracker, Server server, Player* players){
	int socket_id = socket_tracker[player_id];
	int n_clients = server.get_n_clients();
	int player_suggestion, weapon_suggestion, suggested_id;

	int location = players[player_id].get_location();
	int location_suggestion = bridge[location];


	// get player's suggestions (also checks for valid input)
	player_suggestion = get_bounded_input(socket_id, server, request_player, 1, 6);
	weapon_suggestion = get_bounded_input(socket_id, server, request_weapon, 7, 12);


	// at this point, we've gotten valid values for a suggestion
	// communicate to all other clients
	string broadcast_suggestion = 
		"Player " + to_string(player_id) + " suggests: " 
		+ to_string(location_suggestion) + ", " 
		+ to_string(player_suggestion) + ", " 
		+ to_string(weapon_suggestion) + "\n";

	for (int i = 0; i < n_clients; i++){
		if (i != player_id){
			send(socket_tracker[i], broadcast_suggestion.c_str(), broadcast_suggestion.size(), 0);
		}
	}

	// also output on the server
	cout << broadcast_suggestion;


	// now move the suggested player to relevant location
	// according to rules, do this regardless of whether player is active
	suggested_id = player_suggestion - 1;

	if ((suggested_id < n_clients) && (suggested_id != player_id)){
		players[suggested_id].set_location(location);
		players[suggested_id].set_just_suggested(true);


		string move_message = "You are being moved to location " 
			+ to_string(location) + "\n";

		string broadcast_move = "Player " 
			+ to_string(suggested_id) + " is being moved to "
			+ "location " + to_string(location) + "\n";

		// notify player being moved
		send(socket_tracker[suggested_id], move_message.c_str(), move_message.size(), 0);

		// notify everyone else
		for (int i = 0; i < n_clients; i++){
			if (i != (suggested_id)){
				send(socket_tracker[i], broadcast_move.c_str(), broadcast_move.size(), 0);
			}
		}

		// also notify server
		cout << broadcast_move;
	}


	// at this point, suggestion was obtained, and player was moved if needed

	// finally, loop over all other players 
	// (in the same order as turn progression)
	// when one of them shows any card, break
	int temp_id, show_card, card_index, n_overlap, temp_socket;
	string show_card_str, show_card_broadcast, no_show_broadcast, force_show_card;

	for (int i = 1; i < n_clients; i++){
		temp_id = (player_id + i) % n_clients;
		temp_socket = socket_tracker[temp_id];

		vector<int> overlap_cards;

		if (players[temp_id].hand_contains(location_suggestion)){
			overlap_cards.push_back(location_suggestion);
		}
		if (players[temp_id].hand_contains(weapon_suggestion)){
			overlap_cards.push_back(weapon_suggestion);
		}
		if (players[temp_id].hand_contains(player_suggestion)){
			overlap_cards.push_back(player_suggestion);
		}


		
		n_overlap = overlap_cards.size();

		// player does not have any; pass to next player
		if (n_overlap == 0){
			no_show_broadcast = "Player " + to_string(temp_id) + " does not have any "
				+ "of the suggested cards; passing.\r\n";
			for (int j = 0; j < n_clients; j++){
				send(socket_tracker[j], no_show_broadcast.c_str(), 
					 no_show_broadcast.size(), 0);
			}
			cout << no_show_broadcast << endl;

			continue;
		}

		// this isn't very clean but equivalent ','.join code is even messier
		// for applications with more cards we'd use something more robust
		if (n_overlap == 3){
			show_card_str = "Show one of " + to_string(overlap_cards[0]) + ", "
				+ to_string(overlap_cards[1]) + ", or " + to_string(overlap_cards[2]) + ":\r\n\t";

			show_card = get_contained_input(
				temp_socket, server, show_card_str, overlap_cards
			);
			
		}
		else if (n_overlap == 2){
			show_card_str = "Show one of " + to_string(overlap_cards[0])
				+ " or " + to_string(overlap_cards[1]) + ":\r\n\t";

			show_card = get_contained_input(
				temp_socket, server, show_card_str, overlap_cards
			);
			
		}
		else if (n_overlap == 1){
			show_card = overlap_cards[0];

			force_show_card = "You are forced to show card " 
				+ to_string(show_card) + "\r\n";
			
			send(temp_socket, force_show_card.c_str(), 
				 force_show_card.size(), 0);
		}


		// we've gotten the card to show, now actually show, and broadcast
		show_card_broadcast = "Player " + to_string(temp_id) 
			+ " has shown Player " + to_string(player_id) + " a card\n";

		show_card_str = "Player " + to_string(temp_id) 
			+ " has shown you card " + to_string(show_card) + "\n";

		for (int j = 0; j < n_clients; j++){
			if (j == player_id){
				send(socket_tracker[j], show_card_str.c_str(), 
				 	 show_card_str.size(), 0);
			}
			else if (j != temp_id){
				send(socket_tracker[j], show_card_broadcast.c_str(), 
				 	 show_card_broadcast.size(), 0);
			}
		}


		// card was shown; nobody else needs to show anything
		break;


		// testing: show all players' hands
		// Player temp_player = players[temp_id];

		// for (int card_index = 0; card_index < temp_player.get_hand().size(); card_index++){
		// 	cout << to_string(temp_player.get_hand()[card_index]) << endl;
		// }
	}




	return 0;
}



// at the end of this, the relevant player will either win or be deactivated
void GamePlay::accuse(int player_id, int* socket_tracker, Server server, Player* player){
	int socket_id = socket_tracker[player_id];
	int n_clients = server.get_n_clients();
	int accused_player, accused_weapon, accused_location;
	
	
	// get player's accusations
	accused_player = get_bounded_input(socket_id, server, request_player, 1, 6);
	accused_location = get_bounded_input(socket_id, server, accuse_location, 13, 21);
	accused_weapon = get_bounded_input(socket_id, server, request_weapon, 7, 12);


	
	// at this point, we've gotten valid values for the accusation
	// communicate to all other clients
	string accuse_broadcast = 
		"Player " + to_string(player_id) + " accuses: " 
		+ to_string(accused_player) + ", " 
		+ to_string(accused_location) + ", "
		+ to_string(accused_weapon) + "\n";


	for (int i = 0; i < n_clients; i++){
		if (i != player_id){
			send(socket_tracker[i], accuse_broadcast.c_str(), accuse_broadcast.size(), 0);
		}
	}
	// also output on the server
	cout << accuse_broadcast;


	// show accusing player the case file
	send(socket_id, case_file_string.c_str(), case_file_string.size(), 0);

	// now check correctness
	if ((accused_player != case_file[0]) 
		|| (accused_weapon != case_file[1]) 
		|| (accused_location != case_file[2])){

		// if any are wrong, deactivate player and notify everyone
		send(socket_id, wrong_accusation.c_str(), wrong_accusation.size(), 0);
		player->deactivate();

		string deactivated = 
			"Player " + to_string(player_id) + " incorrect; deactivated.\n";

		for (int i = 0; i < n_clients; i++){
			if (i != player_id){
				send(socket_tracker[i], deactivated.c_str(), deactivated.size(), 0);
			}
		}
		cout << deactivated;
	}
	else {
		// otherwise, just set the winning condition
		player->set_win();
	}



	return;
}


// generally, at the end of your turn, you have the option to accuse.
// calling this will let you bypass that option
void GamePlay::pass(int player_id, int* socket_tracker, Server server){
	// just communicate to all other clients
	string pass_broadcast = "Player " + to_string(player_id) 
		+ "\'s turn has ended.\n";

	for (int i = 0; i < server.get_n_clients(); i++){
		if (i != player_id){
			send(socket_tracker[i], pass_broadcast.c_str(), pass_broadcast.size(), 0);
		}
	}

	// also output on the server
	cout << pass_broadcast;
	return;
}



// string GamePlay::getting_accused(int player_id, int* socket_tracker){
// 	cout << "not implemented yet" << endl;
// 	return 0;
// }




// remaining attributes/methods are largely helpers
int GamePlay::get_bounded_input(int socket_id, Server server, string message, int lo, int hi){
	string buffer;
	int output;

	bool success = false;
	while (!success){
		send(socket_id, message.c_str(), message.size(), 0);

		buffer = server.receive_communication(socket_id);
		istringstream(buffer) >> output;

		// validate input (with the GUI this will probably not be necessary,
		// it will be a conditional based on where the player clicks)
		if ((output >= lo) && (output <= hi)){
			success = true;
		}
		else {
			send(socket_id, invalid_input.c_str(), invalid_input.size(), 0);
		}
	}

	return output;
}

int GamePlay::get_contained_input(int socket_id, Server server, 
			string message, vector<int> superset){

	string buffer;
	int output;

	bool success = false;
	while (!success){
		send(socket_id, message.c_str(), message.size(), 0);

		buffer = server.receive_communication(socket_id);
		istringstream(buffer) >> output;

		// validate input (with the GUI this will probably not be necessary,
		// it will be a conditional based on where the player clicks)
		for (int i = 0; i < superset.size(); i++){
			if (superset[i] == output){
				success = true;
				break;
			}
		}
		if (!success){
			send(socket_id, invalid_input.c_str(), invalid_input.size(), 0);
		}
	}

	return output;
}

// get input that is contained within a string array
string GamePlay::get_valid_input(int socket_id, Server server, 
		string message, string outputs[]){
	
	string action;

	bool success = false;
	int i = 0;

	while (!success){
		send(socket_id, message.c_str(), message.size(), 0);

		action = server.receive_communication(socket_id);


		// validate input (with the GUI this will probably not be necessary,
		// it will be a conditional based on where the player clicks)
		while (true){
			if (outputs[i].compare("") == 0){
				// reached end of output array
				break;
			}
			
			if (action.compare(outputs[i]) == 0){
				// if any are present, selection is valid
				success = true;
				break;
			}

			i++;
		}
		
		if (!success){
		    send(socket_id, invalid_input.c_str(), invalid_input.size(), 0);

		    // reset loop
		    i = 0;
		}
	}

	return action;
}

void GamePlay::call_valid_move(int player_id, int* socket_tracker,
		Server server, string action, Player* players){
		// we've gotten valid action; execute corresponding method
	if (action.compare("navigate") == 0){
		navigate(player_id, socket_tracker, server);
	}
	else if (action.compare("suggest") == 0){
		suggest(player_id, socket_tracker, server, players);
	}
	else if (action.compare("accuse") == 0){
		accuse(player_id, socket_tracker, server, &players[player_id]);
	}
	else if (action.compare("pass") == 0){
		pass(player_id, socket_tracker, server);
	}
	else if (action.compare("stay") == 0){
		// don't do anything
	}
	else {
		// data validation did not output valid action
		cerr << "Invalid action not caught by data validation: " 
			 << action 
			 << "; call get_valid_input before this method" << endl;
		exit(1);
	}

	return;
}


void GamePlay::show_hand(int socket_id, Player* player){
	vector<int> hand = player->get_hand();
	string output = "\tYour cards: ";

	for (int i = 0; i < hand.size(); i++){
		output += to_string(hand[i]);

		if (i != hand.size() - 1){
			output += ", ";
		}
	}
	output += "\r\n";


	send(socket_id, output.c_str(), output.size(), 0);
}

void GamePlay::print_state(int socket_id, int n_clients, Player* players){
	// first create output string
	string output = "Player locations: \n\t";

	for (int i = 0; i < n_clients; i++){
		output += "Player " + to_string(i) + ": " 
			   + to_string(players[i].get_location()) + "\n";

		if (i != (n_clients - 1)) output += "\t";
	}

	// then show to player
	send(socket_id, output.c_str(), output.size(), 0);

}

void GamePlay::set_player_character(int player_id, Player* player){
	player->set_character(card_map[player_id + 1]);
}
        


// other static attributes
string GamePlay::request_location = "Where would you like to move?\r\n\t";
string GamePlay::request_player = "Who do you think committed the crime?\r\n\t";
string GamePlay::accuse_location = "In what room?\r\n\t";
string GamePlay::request_weapon = "With what weapon?\r\n\t";

// all used in execute_turn
string GamePlay::start_str = "Your turn.  \n";
string GamePlay::navigate_stay_str = "Do you want to navigate or stay?\r\n\t";
string GamePlay::suggest_accuse_str = "Do you want to suggest, accuse, or pass?\r\n\t";
string GamePlay::accuse_pass_str = "Do you want to accuse or pass?\r\n\t";
string GamePlay::force_navigate_str = "You must move from current location\r\n";
string GamePlay::force_stay_str = "You have no valid moves\r\n\t";
string GamePlay::check_hand = "Check hand? [y]/[n]: ";
string GamePlay::check_state = "Check player locations? [y]/[n]: ";

string GamePlay::invalid_input = "Invalid input, try again:\r\n";

string GamePlay::wrong_accusation = "You guessed incorrectly; deactivating...\n";


// for validating inputs (probably won't need this with a GUI)
// these terminating strings are not great, but it's impossible to
// determine the size of an array using its pointer
string GamePlay::navigate_stay[] = {"navigate", "stay", ""};
string GamePlay::suggest_accuse[] = {"suggest", "accuse", "pass", ""};
string GamePlay::accuse_pass[] = {"accuse", "pass", ""};
string GamePlay::yes_no[] = {"y", "n", ""};

int GamePlay::all_rooms[] = {1, 3, 5, 9, 11, 13, 17, 19, 21};


bool GamePlay::in_room(Player* player){
	int player_location = player->get_location();
	bool result = false;

	// find() method doesn't work with needed compiler version
	for (int i = 0; i < sizeof(all_rooms) / sizeof(all_rooms[0]); i++){
		if (all_rooms[i] == player_location){
			result = true;
			break;
		}
	}

	return result;
}



// it'd be nice to store players here so we don't have to pass it back and forth
// when executing gameplay logic
// but circular includes and forward declarations don't work with this logic
// void GamePlay::finalize_players(Player *final_players){
// 	players = final_players;
// }


// maintain case file
int GamePlay::case_file[3];
string GamePlay::case_file_string;

void GamePlay::populate_case_file(int card1, int card2, int card3){
	case_file[0] = card1;
	case_file[1] = card2;
	case_file[2] = card3;

	// sort so cards are ordered by player, weapon, location
	int n = sizeof(case_file) / sizeof(case_file[0]);
    sort(case_file, case_file + n);

    case_file_string = "Case file: " 
    	+ to_string(case_file[0]) + ", "
    	+ to_string(case_file[1]) + ", " 
    	+ to_string(case_file[2]) + "\n";
}


// store and populate maps for relevant functions here
unordered_map<int, string> GamePlay::location_map;
unordered_map<int, string> GamePlay::card_map;
unordered_map<int, int> GamePlay::bridge;

void GamePlay::populate_location_map(){
	// unordered_map<int, string> locations;
	location_map[1] = "Study";
	location_map[2] = "Hallway (Study, Hall)";
	location_map[3] = "Hall";
	location_map[4] = "Hallway (Hall, Lounge)";
	location_map[5] = "Lounge";
	location_map[6] = "Hallway (Study, Library)";
	location_map[7] = "Hallway (Hall, Billard Room)";
	location_map[8] = "Hallway (Lounge, Dining)";
	location_map[9] = "Library";
	location_map[10] = "Hallway (Library, Billard Room)";
	location_map[11] = "Billard Room";
	location_map[12] = "Hallway (Billard Room, Dining)";
	location_map[13] = "Dining Room";
	location_map[14] = "Hallway (Library, Conservatory)";
	location_map[15] = "Hallway (Billard Room, Ballroom)";
	location_map[16] = "Hallway (Dining, Kitchen)";
	location_map[17] = "Conservatory";
	location_map[18] = "Hallway (Conservatory, Ballroom)";
	location_map[19] = "Ballroom";
	location_map[20] = "Hallway (Ballroom, Kitchen)";
	location_map[21] = "Kitchen";
}

void GamePlay::populate_card_map(){
	// using 0 indexed would play better with player_id
	// but some string - int conversion methods return 0 when invalid
	card_map[1] = "Miss Scarlet";
	card_map[2] = "Professor Plum";	
	card_map[3] = "Mrs. Peacock";
	card_map[4] = "Mr. Green";
	card_map[5] = "Mrs. White";
	card_map[6] = "Colonel Mustard";

	card_map[7] = "Rope";
	card_map[8] = "Lead Pipe";
	card_map[9] = "Knife";
	card_map[10] = "Wrench";
	card_map[11] = "Candlestick";
	card_map[12] = "Revolver";

	card_map[13] = "Study";
	card_map[14] = "Hall";
	card_map[15] = "Lounge";
	card_map[16] = "Library";
	card_map[17] = "Billard Room";
	card_map[18] = "Dining Room";
	card_map[19] = "Conservatory";
	card_map[20] = "Ballroom";
	card_map[21] = "Kitchen";

}

void GamePlay::populate_bridge(){
	// bridge from location rooms to card rooms
	bridge[1] = 13;
	bridge[3] = 14;
	bridge[5] = 15;
	bridge[9] = 16;
	bridge[11] = 17;
	bridge[13] = 18;
	bridge[17] = 19;
	bridge[19] = 20;
	bridge[21] = 21;
}







