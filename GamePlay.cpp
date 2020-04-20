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



	send(socket_id, start_str.c_str(), start_str.size(), 0);


	// check hand and locations here
	yn = get_valid_input(socket_id, server, check_hand, yes_no);
	if (yn == "y") show_hand(socket_id, player);

	yn = get_valid_input(socket_id, server, check_state, yes_no);
	if (yn == "y") print_state(socket_id, n_clients, players);



	// now get valid moves
	// these are adjacent spots, plus secret passageways, 
	// minus occupied hallways
	vector<int> valid_moves_int = valid_move_map[player->get_location()];

	for (int id = 0; id < n_clients; id++){
		if ((id == player_id) || in_room(&players[id])){
			continue;
		}

		// if another player in hallway, remove from valid moves if necessary
		int temp_location = players[id].get_location();

		// none of these other options modify the size of the vector, which we need
	    // vector<int>::iterator new_end;
	    // new_end = remove(valid_moves.begin(), valid_moves.end(), temp_location);
        // remove_copy(valid_moves.begin(), valid_moves.end(), valid_moves.begin(), temp_location);

    	valid_moves_int.erase(
    		remove(valid_moves_int.begin(), valid_moves_int.end(), temp_location), 
    		valid_moves_int.end()
    	);
	}




	string move;
	bool done = false;
	vector<string> valid_moves;

	string move_possibilities = "Possible locations to navigate: ";
	bool any_valid_moves = (valid_moves_int.size() > 0);

	
	// there's 4 possibilities: 2x2 of (any valid moves) x (just got suggested)
	// 3 of them (all except (no valid moves) x (not just suggested))
	// will have the same suggest/accuse/pass at the end, so 
	// to reduce duplicated code, stick them together
	// the final block (no valid moves) x (not just suggested)
	// goes directly to accuse/pass

	if (any_valid_moves || player->get_just_suggested()){
		// let player know what choices they have for navigation
		// before they choose their move (if any)
		// convert location ints to strings while making message to send
		for (int i = 0; i < valid_moves_int.size(); i++){
			string temp = location_map[valid_moves_int[i]];
			valid_moves.push_back(temp);

			move_possibilities += temp;

			if (i != valid_moves_int.size() - 1){
				move_possibilities += "; ";
			}
			else {
				move_possibilities += "\r\n";
			}
		}
		send(socket_id, move_possibilities.c_str(), 
			 move_possibilities.size(), 0);


		// now give them available options
		if (!player->get_just_suggested()){	
			// force them to navigate or accuse
			move = get_valid_input(socket_id, server, 
				navigate_accuse_str, navigate_accuse);

			// either navigate or accuse
			call_valid_move(
				player_id, socket_tracker, valid_moves, server, move, players
			);
			if (move.compare("accuse") == 0){
				done = true;
			}
		}
		else if (any_valid_moves){
			// this implies they were just suggested
			// so they're allowed to stay if desired
			move = get_valid_input(socket_id, server, 
				navigate_stay_str, navigate_stay);

			// either navigate or stay
			if (move.compare("navigate") == 0){
				navigate(player_id, valid_moves, socket_tracker, 
						 server, &players[player_id]);
			}
		}
		else {
			// just suggested and no valid moves
			send(socket_id, force_stay_str.c_str(), 
				 force_stay_str.size(), 0);
		}


		// navigation is finished at this point (if possible)
		// now they can suggest/pass/accuse
		if (!done && in_room(player)){
			// suggest, accuse, or pass logic
			move = get_valid_input(socket_id, server, 
				suggest_accuse_str, suggest_accuse);
			call_valid_move(
				player_id, socket_tracker, valid_moves, server, move, players
			);

			
			// then, if suggested, accuse or pass options again
			if (move.compare("suggest") == 0){
				move = get_valid_input(socket_id, server, 
					accuse_pass_str, accuse_pass);

				call_valid_move(
					player_id, socket_tracker, valid_moves, server, move, players
				);
			}
		}
		else if (!done){
			// directly accuse or pass if not in room
			move = get_valid_input(socket_id, server, 
				accuse_pass_str, accuse_pass);

			call_valid_move(
				player_id, socket_tracker, valid_moves, server, move, players
			);
			
		}

	}
	else {	// no valid moves AND not just suggested
		send(socket_id, force_stay_str.c_str(), 
			 force_stay_str.size(), 0);	

		// directly accuse or pass
		move = get_valid_input(socket_id, server, 
			accuse_pass_str, accuse_pass);

		call_valid_move(
			player_id, socket_tracker, valid_moves, server, move, players
		);
	}

	
	// set just_suggested attribute to false at end of turn
	player->set_just_suggested(false);

	// and that's it.
	return ;
}




void GamePlay::navigate(int player_id, vector<string> valid_moves, 
		int* socket_tracker, Server server, Player* player){

	int socket_id = socket_tracker[player_id];
	int n_clients = server.get_n_clients();
	string move_broadcast, final_location;

	// logic differs depending on how many valid moves there are
	if (valid_moves.size() == 0){
		cerr << "No valid moves, logical mistake in execute_turn structure" << endl;
		exit(1);
	}
	else if (valid_moves.size() == 1){
		// only one option; force move
		final_location = valid_moves[0];

		string force_move = "Only one choice for navigation.\r\n"
			"Automatically moving you to the " + final_location + "\r\n";

		send(socket_id, force_move.c_str(), force_move.size(), 0);
	}
	else {
		// more than one option; let player choose
		// but force it to be one of the entries in valid_moves
		final_location = get_contained_input(socket_id, server, 
			request_location, valid_moves);
	}


	// now actually move
	player->set_location(reverse_location_map[final_location]);
	

	// and communicate to all other clients
	move_broadcast = card_map[player_id + 1] 
		+ " is moving to the " + final_location + "\n";

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
	int suggested_id;
	string player_suggestion, weapon_suggestion;


	int location = players[player_id].get_location();
	int location_card = bridge[location];
	string location_suggestion = location_map[location];


	// get player's suggestions (also checks for valid input)
	player_suggestion = get_bounded_input(socket_id, server, request_player, 1, 6);
	weapon_suggestion = get_bounded_input(socket_id, server, request_weapon, 7, 12);


	// at this point, we've gotten valid values for a suggestion
	// communicate to all other clients
	string broadcast_suggestion = 
		card_map[player_id + 1] + " suggests: " 
		+ player_suggestion + ", " 
		+ weapon_suggestion + ", " 
		+ location_suggestion + "\n";

	for (int i = 0; i < n_clients; i++){
		if (i != player_id){
			send(socket_tracker[i], broadcast_suggestion.c_str(), broadcast_suggestion.size(), 0);
		}
	}

	// also output on the server
	cout << broadcast_suggestion;


	// now move the suggested player to relevant location
	// according to rules, do this regardless of whether player is active
	suggested_id = reverse_card_map[player_suggestion] - 1;

	if ((suggested_id < n_clients) && (suggested_id != player_id)){
		players[suggested_id].set_location(location);
		players[suggested_id].set_just_suggested(true);


		string move_message = "You are being moved to the " 
			+ location_suggestion + "\n";

		string broadcast_move = player_suggestion
			+ " is being moved to the " + location_suggestion + "\n";
			

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
	int temp_id, card_index, n_overlap, temp_socket;
	string show_card;
	string show_card_str, show_card_broadcast, no_show_broadcast, force_show_card;

	for (int i = 1; i < n_clients; i++){
		temp_id = (player_id + i) % n_clients;
		temp_socket = socket_tracker[temp_id];

		vector<string> overlap_cards;

		if (players[temp_id].hand_contains(location_card)){
			overlap_cards.push_back(location_suggestion);
		}
		if (players[temp_id].hand_contains(reverse_card_map[weapon_suggestion])){
			overlap_cards.push_back(weapon_suggestion);
		}
		if (players[temp_id].hand_contains(reverse_card_map[player_suggestion])){
			overlap_cards.push_back(player_suggestion);
		}


		
		n_overlap = overlap_cards.size();

		// player does not have any; pass to next player
		if (n_overlap == 0){
			no_show_broadcast = card_map[temp_id + 1] + " does not have any "
				+ "of the suggested cards.\r\n";
			
			for (int j = 0; j < n_clients; j++){
				if (j == temp_id){
					send(socket_tracker[j], no_show_individual.c_str(), 
					 	 no_show_individual.size(), 0);
				}
				else {
					send(socket_tracker[j], no_show_broadcast.c_str(), 
					 	 no_show_broadcast.size(), 0);
				}				
				
			}
			cout << no_show_broadcast << endl;

			continue;
		}

		// this isn't very clean but equivalent ','.join code is even messier
		// for applications with more cards we'd use something more robust
		if (n_overlap == 3){
			show_card_str = "Show one of " + overlap_cards[0] + ", "
				+ overlap_cards[1] + ", or " + overlap_cards[2] + ":\r\n\t";

			show_card = get_contained_input(
				temp_socket, server, show_card_str, overlap_cards
			);
			
		}
		else if (n_overlap == 2){
			show_card_str = "Show one of " + overlap_cards[0]
				+ " or " + overlap_cards[1] + ":\r\n\t";

			show_card = get_contained_input(
				temp_socket, server, show_card_str, overlap_cards
			);
			
		}
		else if (n_overlap == 1){
			show_card = overlap_cards[0];

			force_show_card = "You are forced to show card: " 
				+ show_card + "\r\n";
			
			send(temp_socket, force_show_card.c_str(), 
				 force_show_card.size(), 0);
		}


		// we've gotten the card to show, now actually show, and broadcast
		show_card_broadcast = card_map[temp_id + 1] 
			+ " has shown " + card_map[player_id + 1] + " a card\n";

		show_card_str = card_map[temp_id + 1] 
			+ " has shown you a card: " + show_card + "\n";

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
	string accused_player, accused_weapon, accused_location;
	
	
	// get player's accusations
	accused_player = get_bounded_input(socket_id, server, request_player, 1, 6);
	accused_weapon = get_bounded_input(socket_id, server, request_weapon, 7, 12);
	accused_location = get_bounded_input(socket_id, server, accuse_location, 13, 21);


	
	// at this point, we've gotten valid values for the accusation
	// communicate to all other clients
	string accuse_broadcast = 
		card_map[player_id + 1] + " accuses: " 
		+ accused_player + ", " 
		+ accused_weapon + ", "
		+ accused_location + "\n";


	for (int i = 0; i < n_clients; i++){
		if (i != player_id){
			send(socket_tracker[i], accuse_broadcast.c_str(), accuse_broadcast.size(), 0);
		}
	}
	// also output on the server
	cout << accuse_broadcast;


	// now check correctness
	if ((reverse_card_map[accused_player] != case_file[0])
		|| (reverse_card_map[accused_weapon] != case_file[1]) 
		|| (reverse_card_map[accused_location] != case_file[2])){

		// if any are wrong, they still get to see the case file
		send(socket_id, case_file_string.c_str(), case_file_string.size(), 0);

		// then, deactivate player and notify everyone
		send(socket_id, wrong_accusation.c_str(), wrong_accusation.size(), 0);
		player->deactivate();

		string deactivated = 
			card_map[player_id + 1] + " incorrect; deactivated.\n";

		for (int i = 0; i < n_clients; i++){
			if (i != player_id){
				send(socket_tracker[i], deactivated.c_str(), deactivated.size(), 0);
			}
		}
		cout << deactivated;
	}
	else {
		// otherwise, set the winning condition
		player->set_win();

		// game over, so we can show everybody the case file
		for (int i = 0; i < n_clients; i++){
			send(socket_tracker[i], case_file_string.c_str(), case_file_string.size(), 0);
		}

	}


	return;
}


// generally, at the end of your turn, you have the option to accuse.
// calling this will let you bypass that option
void GamePlay::pass(int player_id, int* socket_tracker, Server server){
	// just communicate to all other clients
	string pass_broadcast = card_map[player_id + 1]
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
string GamePlay::get_bounded_input(int socket_id, Server server, string message, int lo, int hi){
	string input;
	int bound;

	bool success = false;
	while (!success){
		send(socket_id, message.c_str(), message.size(), 0);

		input = server.receive_communication(socket_id);


		// validate input (with the GUI this will probably not be necessary,
		// it will be a conditional based on where the player clicks)
		
		if (reverse_card_map.count(input) == 1){
			bound = reverse_card_map[input];

			if ((bound >= lo) && (bound <= hi)){
				success = true;
			}

		}
		
		if (!success){
			send(socket_id, invalid_input.c_str(), invalid_input.size(), 0);
		}
	}

	return input;
}

string GamePlay::get_contained_input(int socket_id, Server server, 
			string message, vector<string> superset){

	string input;

	bool success = false;
	while (!success){
		send(socket_id, message.c_str(), message.size(), 0);

		input = server.receive_communication(socket_id);

		// validate input (with the GUI this will probably not be necessary,
		// it will be a conditional based on where the player clicks)
		for (int i = 0; i < superset.size(); i++){
			if (superset[i].compare(input) == 0){
				success = true;
				break;
			}
		}
		if (!success){
			send(socket_id, invalid_input.c_str(), invalid_input.size(), 0);
		}
	}

	return input;
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
		vector<string> valid_moves, Server server, string action, Player* players){

	// we've gotten valid action; execute corresponding method
	if (action.compare("navigate") == 0){
		navigate(player_id, valid_moves, socket_tracker, server, &players[player_id]);
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
		output += card_map[hand[i]];

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
		output += card_map[i + 1] + ": " 
			   + location_map[players[i].get_location()] + "\n";

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
string GamePlay::navigate_accuse_str = "Do you want to navigate or accuse?\r\n\t";
string GamePlay::force_stay_str = "You have no valid moves\r\n\t";
string GamePlay::check_hand = "Check hand? [y]/[n]: ";
string GamePlay::check_state = "Check player locations? [y]/[n]: ";

string GamePlay::invalid_input = "Invalid input, try again:\r\n";

string GamePlay::wrong_accusation = "You guessed incorrectly; deactivating...\n";
string GamePlay::no_show_individual = "You do not have any of the suggested cards.\r\n";

// for validating inputs (probably won't need this with a GUI)
// these terminating strings are not great, but it's impossible to
// determine the size of an array using its pointer
string GamePlay::navigate_stay[] = {"navigate", "stay", ""};
string GamePlay::navigate_accuse[] = {"navigate", "accuse", ""};
string GamePlay::suggest_accuse[] = {"suggest", "accuse", "pass", ""};
string GamePlay::accuse_pass[] = {"accuse", "pass", ""};
string GamePlay::yes_no[] = {"y", "n", ""};

int GamePlay::all_rooms[] = {1, 3, 5, 9, 11, 13, 17, 19, 21};	// locations


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
    	+ card_map[case_file[0]] + ", "
    	+ card_map[case_file[1]] + ", " 
    	+ card_map[case_file[2]] + "\n";
}


// store and populate maps for relevant functions here
unordered_map<int, string> GamePlay::location_map;
unordered_map<string, int> GamePlay::reverse_location_map;

unordered_map<int, string> GamePlay::card_map;
unordered_map<string, int> GamePlay::reverse_card_map;

unordered_map< int, vector<int> > GamePlay::valid_move_map;
unordered_map<int, int> GamePlay::bridge;


void GamePlay::populate_location_map(){
	valid_move_map[1].push_back(2);
	valid_move_map[1].push_back(6);
	valid_move_map[1].push_back(21);	// secret passageway

	valid_move_map[2].push_back(1);
	valid_move_map[2].push_back(3);
	
	valid_move_map[3].push_back(2);
	valid_move_map[3].push_back(4);
	valid_move_map[3].push_back(7);

	valid_move_map[4].push_back(3);
	valid_move_map[4].push_back(5);

	valid_move_map[5].push_back(4);
	valid_move_map[5].push_back(8);
	valid_move_map[5].push_back(17);	// secret passageway

	valid_move_map[6].push_back(1);
	valid_move_map[6].push_back(9);

	valid_move_map[7].push_back(3);
	valid_move_map[7].push_back(11);

	valid_move_map[8].push_back(5);
	valid_move_map[8].push_back(13);

	valid_move_map[9].push_back(6);
	valid_move_map[9].push_back(10);
	valid_move_map[9].push_back(14);

	valid_move_map[10].push_back(9);
	valid_move_map[10].push_back(11);

	valid_move_map[11].push_back(10);
	valid_move_map[11].push_back(12);
	valid_move_map[11].push_back(7);
	valid_move_map[11].push_back(15);

	valid_move_map[12].push_back(11);
	valid_move_map[12].push_back(13);

	valid_move_map[13].push_back(12);
	valid_move_map[13].push_back(8);
	valid_move_map[13].push_back(16);

	valid_move_map[14].push_back(9);
	valid_move_map[14].push_back(17);
	
	valid_move_map[15].push_back(11);
	valid_move_map[15].push_back(19);

	valid_move_map[16].push_back(13);
	valid_move_map[16].push_back(21);

	valid_move_map[17].push_back(14);
	valid_move_map[17].push_back(18);
	valid_move_map[17].push_back(5);	// secret passageway

	valid_move_map[18].push_back(17);
	valid_move_map[18].push_back(19);

	valid_move_map[19].push_back(18);
	valid_move_map[19].push_back(20);
	valid_move_map[19].push_back(15);

	valid_move_map[20].push_back(19);
	valid_move_map[20].push_back(21);
	
	valid_move_map[21].push_back(20);
	valid_move_map[21].push_back(16);
	valid_move_map[21].push_back(1);	// secret passageway




	// unordered_map<int, string> locations;
	location_map[1] = "Study";
	location_map[2] = "Hallway (Study, Hall)";
	location_map[3] = "Hall";
	location_map[4] = "Hallway (Hall, Lounge)";
	location_map[5] = "Lounge";
	location_map[6] = "Hallway (Study, Library)";
	location_map[7] = "Hallway (Hall, Billiard Room)";
	location_map[8] = "Hallway (Lounge, Dining)";
	location_map[9] = "Library";
	location_map[10] = "Hallway (Library, Billiard Room)";
	location_map[11] = "Billiard Room";
	location_map[12] = "Hallway (Billiard Room, Dining)";
	location_map[13] = "Dining Room";
	location_map[14] = "Hallway (Library, Conservatory)";
	location_map[15] = "Hallway (Billiard Room, Ballroom)";
	location_map[16] = "Hallway (Dining, Kitchen)";
	location_map[17] = "Conservatory";
	location_map[18] = "Hallway (Conservatory, Ballroom)";
	location_map[19] = "Ballroom";
	location_map[20] = "Hallway (Ballroom, Kitchen)";
	location_map[21] = "Kitchen";

	// this is horrible, but unfortunately map iteration only 
	// works on compilers that don't work with fd_set
	reverse_location_map["Study"] = 1;
	reverse_location_map["Hallway (Study, Hall)"] = 2;
	reverse_location_map["Hall"] = 3;
	reverse_location_map["Hallway (Hall, Lounge)"] = 4;
	reverse_location_map["Lounge"] = 5;
	reverse_location_map["Hallway (Study, Library)"] = 6;
	reverse_location_map["Hallway (Hall, Billiard Room)"] = 7;
	reverse_location_map["Hallway (Lounge, Dining)"] = 8;
	reverse_location_map["Library"] = 9;
	reverse_location_map["Hallway (Library, Billiard Room)"] = 10;
	reverse_location_map["Billiard Room"] = 11;
	reverse_location_map["Hallway (Billiard Room, Dining)"] = 12;
	reverse_location_map["Dining Room"] = 13;
	reverse_location_map["Hallway (Library, Conservatory)"] = 14;
	reverse_location_map["Hallway (Billiard Room, Ballroom)"] = 15;
	reverse_location_map["Hallway (Dining, Kitchen)"] = 16;
	reverse_location_map["Conservatory"] = 17;
	reverse_location_map["Hallway (Conservatory, Ballroom)"] = 18;
	reverse_location_map["Ballroom"] = 19;
	reverse_location_map["Hallway (Ballroom, Kitchen)"] = 20;
	reverse_location_map["Kitchen"] = 21;

	// ideally this would be implemented as a graph, 
	// with 4 possible directions to go (e.g. node.left, node.right, etc).
	// or we could design separate study and hallway types, and use those
	// (e.g. a hallway could contain attributes for which rooms they're 
	// connected to, and similarly with the rooms)
	// but using a map is easier

    
}

void GamePlay::populate_card_map(){
	// using 0 indexed would play better with player_id
	// but some string - int conversion methods return 0 when invalid
	card_map[1] = "Miss Scarlet";
	card_map[2] = "Prof. Plum";	
	card_map[3] = "Mrs. Peacock";
	card_map[4] = "Mr. Green";
	card_map[5] = "Mrs. White";
	card_map[6] = "Col. Mustard";

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
	card_map[17] = "Billiard Room";
	card_map[18] = "Dining Room";
	card_map[19] = "Conservatory";
	card_map[20] = "Ballroom";
	card_map[21] = "Kitchen";

	reverse_card_map["Miss Scarlet"] = 1;
	reverse_card_map["Prof. Plum"] = 2;
	reverse_card_map["Mrs. Peacock"] = 3;
	reverse_card_map["Mr. Green"] = 4;
	reverse_card_map["Mrs. White"] = 5;
	reverse_card_map["Col. Mustard"] = 6;

	reverse_card_map["Rope"] = 7;
	reverse_card_map["Lead Pipe"] = 8;
	reverse_card_map["Knife"] = 9;
	reverse_card_map["Wrench"] = 10;
	reverse_card_map["Candlestick"] = 11;
	reverse_card_map["Revolver"] = 12;

	reverse_card_map["Study"] = 13;
	reverse_card_map["Hall"] = 14;
	reverse_card_map["Lounge"] = 15;
	reverse_card_map["Library"] = 16;
	reverse_card_map["Billiard Room"] = 17;
	reverse_card_map["Dining Room"] = 18;
	reverse_card_map["Conservatory"] = 19;
	reverse_card_map["Ballroom"] = 20;
	reverse_card_map["Kitchen"] = 21;



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







