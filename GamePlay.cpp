#include "GamePlay.h"
#include "globals.h"

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
	string yn, confirm;
	bool kickoff = false;


	// kick off client turn
	send(socket_id, start_str.c_str(), start_str.size(), 0);

	// get confirmation before continuing
	while (!kickoff){
		confirm = server.receive_communication(socket_id);
		if (confirm.compare("confirmed.") == 0) kickoff = true;
	}
	





	// check hand and locations here
	// this is not necessary with the GUI

	// yn = get_valid_input(socket_id, server, check_hand, yes_no);
	// if (yn == "y") show_hand(socket_id, player);

	// yn = get_valid_input(socket_id, server, check_state, yes_no);
	// if (yn == "y") print_state(socket_id, n_clients, players);



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

		/*

		THIS IS NOT REALLY NECESSARY WITH THE GUI EITHER
		WE WILL PREFERABLY HIGHLIGHT VALID MOVES
		*/
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
		// send(socket_id, move_possibilities.c_str(), 
		// 	 move_possibilities.size(), 0);
		
		// */

		// now give them available options
		if (!player->get_just_suggested()){	
			// force them to navigate or accuse
			// cout << "getting move" << endl;

			move = get_valid_input(socket_id, server, 
				navigate_accuse_str, navigate_accuse);

			// cout << "plz: " << move << endl;

			// either navigate or accuse
			call_valid_move(
				player_id, socket_tracker, valid_moves, server, move, players
			);
			if (move.compare(accuse_str) == 0){
				done = true;
			}
		}
		else if (any_valid_moves){
			// this implies they were just suggested
			// so they're allowed to stay if desired
			move = get_valid_input(socket_id, server, 
				navigate_stay_str, navigate_stay);

			// either navigate or stay
			if (move.compare(navigate_str) == 0){
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
			if (move.compare(suggest_str) == 0){
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


		// string force_move = "Only one choice for navigation.\r\n"
		// 	"Automatically moving you to the " + final_location + "\r\n";

		// for the gui, we just need to know whether move was forced
		// we don't need to know where it was to
		send(socket_id, force_move.c_str(), force_move.size(), 0);	// don't need this in the gui
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

		// we don't need to broadcast this; the suggestion broadcast contains
		// all the info the client needs to know
		// the client automatically will move players when they are suggested

		// string move_message = "You are being moved to the " 
		// 	+ location_suggestion + "\n";

		// string broadcast_move = player_suggestion
		// 	+ " is being moved to the " + location_suggestion + "\n";
			

		// // notify player being moved
		// send(socket_tracker[suggested_id], move_message.c_str(), move_message.size(), 0);

		// // notify everyone else
		// for (int i = 0; i < n_clients; i++){
		// 	if (i != (suggested_id)){
		// 		send(socket_tracker[i], broadcast_move.c_str(), broadcast_move.size(), 0);
		// 	}
		// }

		// also notify server
		// cout << broadcast_move;
	}


	// at this point, suggestion was obtained, and player was moved if needed

	// finally, loop over all other players 
	// (in the same order as turn progression)
	// when one of them shows any card, break
	int temp_id, card_index, n_overlap, temp_socket;
	string show_card;
	string show_card_str, show_card_broadcast, no_show_broadcast, force_show_card;
	bool finish = false;

	for (int i = 1; i < n_clients; i++){
		temp_id = (player_id + i) % n_clients;
		temp_socket = socket_tracker[temp_id];

		vector<string> overlap_cards;

		// push them in order
		if (players[temp_id].hand_contains(reverse_card_map[player_suggestion])){
			overlap_cards.push_back(player_suggestion);
		}
		if (players[temp_id].hand_contains(reverse_card_map[weapon_suggestion])){
			overlap_cards.push_back(weapon_suggestion);
		}
		if (players[temp_id].hand_contains(location_card)){
			overlap_cards.push_back(location_suggestion);
		}
		
		
		n_overlap = overlap_cards.size();

		// player does not have any; pass to next player
		if (n_overlap == 0){
			no_show_broadcast = "No cards:" + card_map[temp_id + 1];
			
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
			show_card_str = "Show:" + overlap_cards[0] + ";"
				+ overlap_cards[1] + ";" + overlap_cards[2] + "";

			show_card = get_contained_input(
				temp_socket, server, show_card_str, overlap_cards
			);
			
		}
		else if (n_overlap == 2){
			show_card_str = "Show:" + overlap_cards[0]
				+ ";" + overlap_cards[1] + "";

			show_card = get_contained_input(
				temp_socket, server, show_card_str, overlap_cards
			);

			cout << "got contained input " << show_card << endl;
			
		}
		else if (n_overlap == 1){
			// don't force show, since that conveys information
			show_card_str = "Show:" + overlap_cards[0] + ";"
				+ overlap_cards[1] + ";" + overlap_cards[2] + "";

			show_card = get_contained_input(
				temp_socket, server, show_card_str, overlap_cards
			);

			// show_card = overlap_cards[0];

			// force_show_card = "You are forced to show card: " 
			// 	+ show_card + "\r\n";
			
			// send(temp_socket, force_show_card.c_str(), 
			// 	 force_show_card.size(), 0);
		}
		else {
			cerr << "error: more than 3 cards matching on suggest!" << endl;
		}


		// we've gotten the card to show, now actually show, and broadcast
		show_card_broadcast = "Show broadcast:" + card_map[temp_id + 1] 
			+ ";" + card_map[player_id + 1];

		show_card_str = "Look:" + card_map[temp_id + 1] 
			+ ";" + show_card;

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

		cout << "waiting for confirmation " << endl;

		finish = get_matching_input(socket_tracker[player_id], server, end_turn_str);

		cout << "got confirmation " << endl;


		// card was shown; nobody else needs to show anything
		return 0;


		// testing: show all players' hands
		// Player temp_player = players[temp_id];

		// for (int card_index = 0; card_index < temp_player.get_hand().size(); card_index++){
		// 	cout << to_string(temp_player.get_hand()[card_index]) << endl;
		// }
	}

	// if we get here, nobody had the card. for ease of handling, let the suggesting player know
	send(socket_tracker[player_id], nobody_showed.c_str(), 
	 	 nobody_showed.size(), 0);

	// get confirmation
	finish = get_matching_input(socket_tracker[player_id], server, end_turn_str);





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
	// confirm turn ended for current player (so gui can end turn)
	send(socket_tracker[player_id], turn_end_str.c_str(), turn_end_str.size(), 0);


	// communicate to all other clients
	string pass_broadcast = card_map[player_id + 1]
		+ "\'s turn has ended.\n";


	// not really necessary with the gui 

	// for (int i = 0; i < server.get_n_clients(); i++){
	// 	if (i != player_id){
	// 		// send(socket_tracker[i], pass_broadcast.c_str(), pass_broadcast.size(), 0);
	// 	}
	// 	else {
	// 		send(socket_tracker[i], turn_end.c_str(), turn_end.size(), 0);
	// 	}
	// }

	// also output on the server
	cout << pass_broadcast;
	return;
}



// string GamePlay::getting_accused(int player_id, int* socket_tracker){
// 	cout << "not implemented yet" << endl;
// 	return 0;
// }




// remaining attributes/methods are largely helpers

// get input bounded between two ints (return as string)
string GamePlay::get_bounded_input(int socket_id, Server server, string message, int lo, int hi){
	string input;
	int bound;

	bool success = false;
	bool sent = false;

	while (!success){
		// only send once
		if (!sent){
			send(socket_id, message.c_str(), message.size(), 0);
			sent = true;
		}
		

		input = server.receive_communication(socket_id);

		cout << "received from client: " << input << endl;


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


// get input that's contained within a (string) vector
string GamePlay::get_contained_input(int socket_id, Server server, 
			string message, vector<string> superset){

	string input;

	bool success = false;
	bool sent = false;

	while (!success){
		// only ask client once
		if (!sent){
			send(socket_id, message.c_str(), message.size(), 0);
			sent = true;
		}
		

		input = server.receive_communication(socket_id);

		// validate input
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

// basically used to halt gameplay until player sends this message
bool GamePlay::get_matching_input(int socket_id, Server server, 
		string match_str){

	string input;
	bool success = false;
	
	while (!success){
		input = server.receive_communication(socket_id);

		// validate input
		if (match_str.compare(input) == 0){
			success = true;
		}
	}

	return true;
}


// get input that is contained within a string array
// these are now being generated by the gui
string GamePlay::get_valid_input(int socket_id, Server server, 
		string message, string outputs[]){
	
	string action;

	bool success = false;
	bool sent = false;
	int i = 0;

	while (!success){
		// only send message to client once
		if (!sent){
			send(socket_id, message.c_str(), message.size(), 0);
			sent = true;
		}
		
		// then get answer from client until valid one is passed
		// this is now handled by the gui
		action = server.receive_communication(socket_id);

		// for debugging
		// cout << "received string: " << action << endl;

		// validate input (these are now generated by gui events)
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

		    // if answer invalid, repeat the loop
		    i = 0;
		}
	}

	return action;
}


// basicaly a switch case to call the relevant move
void GamePlay::call_valid_move(int player_id, int* socket_tracker, 
		vector<string> valid_moves, Server server, string action, Player* players){

	// we've gotten valid action; execute corresponding method
	if (action.compare(navigate_str) == 0){
		navigate(player_id, valid_moves, socket_tracker, server, &players[player_id]);
	}
	else if (action.compare(suggest_str) == 0){
		suggest(player_id, socket_tracker, server, players);
	}
	else if (action.compare(accuse_str) == 0){
		accuse(player_id, socket_tracker, server, &players[player_id]);
	}
	else if (action.compare(pass_str) == 0){
		pass(player_id, socket_tracker, server);
	}
	else if (action.compare(stay_str) == 0){
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








