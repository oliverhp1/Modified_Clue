#include "GamePlay.h"

using namespace std;



/** These are all the actions that a player might take on their turn.
 * They are implemented as static methods so we don't have to worry about
 * keeping track of a separate object instance and calling from it.
 * The overall driver is a method 'execute_turn', which calls the other methods
 * Methods correspond to general Clue gameplay:
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
	// ask what player wants to do
	string message = "Your turn.  \n"
		"Do you want to navigate, suggest, accuse, or pass?\r\n\t";

	string invalid_choice = "Invalid choice. Choose again: \n\t";


	// should add an option here to check hand again
	// also perhaps to check all player locations
	

	int* socket_tracker = server.get_socket_tracker();
	int socket_id = socket_tracker[player_id];
	
	send(
		socket_id, 
		message.c_str(), 
		message.size(), 
		0
	);

	// get response
	bool turn_complete = false;

	while (!turn_complete){
		turn_complete = true;	// assume it passes; modify otherwise

		string action_buffer = server.receive_communication(socket_id);

		// switch case would be nice, but strings are not supported
		// perhaps pass in ints later when we use the GUI, then we can do switch case

		if (action_buffer.compare("navigate") == 0){
			navigate(player_id, socket_tracker, server);
		}
		else if (action_buffer.compare("suggest") == 0){
			suggest(player_id, socket_tracker, server, players);
		}
		else if (action_buffer.compare("accuse") == 0){
			accuse(player_id, socket_tracker, server);
		}
		else if (action_buffer.compare("pass") == 0){
			pass(player_id, socket_tracker, server);
		}
		else if (action_buffer.compare("I win") == 0){
			players[player_id].set_win();
		}
		else {
			turn_complete = false;
			
			send(
				socket_id,
				invalid_choice.c_str(),
				invalid_choice.size(),
				0
			);
		}
	}


	return ;
}




void GamePlay::navigate(int player_id, int* socket_tracker, Server server){
	int socket_id = socket_tracker[player_id];
	int n_clients = server.get_n_clients();


	// TODO: FIGURE OUT VALID CHOICES TO MOVE TO BEFOREHAND. 
	// please do this in the Player.cpp file, under execute_turn() method if possible.
	// you'll probably want to make an attribute (e.g. static attribute in GamePlay) tracking where all locations are.
	// for the logic, refer to the diagram and rules in the file describing our class project.
	// you can use location_map.  store a player's current location in an attribute, 
	// e.g. player::location (preferably, make it a private attribute and use setter and getter methods),
	// and then use that and the location_map to determine valid places to move.
	// then stick it in "request_location" string below.
	// syntax for location map is just: location_map[1] will return "Study", and so on. (see bottom of this file)
	// 
	// e.g. "Would you like to move to the " + option1 + " or"... etc

	send(
		socket_id, 
		request_location.c_str(), 
		request_location.size(), 
		0
	);

	// get string representing where player wants to move
	// location_buffer will store the input the player enters in the console
	string location_buffer = server.receive_communication(socket_id);


	/* TODO: 
		first check that the choice was valid.
			can reuse the valid choices you found from earlier
		then, actually move the player there.
		do this by updating the player attribute for location
	*/


	// communicate to all other clients
	string notify_all_clients = "Player " + to_string(player_id) 
		+ " is moving to the " + location_buffer + "\n";

	for (int i = 0; i < n_clients; i++){
		if (i != player_id){
			send(
				socket_tracker[i],
				notify_all_clients.c_str(),
				notify_all_clients.size(),
				0
			);
		}
	}

	// also output on the server
	cout << notify_all_clients;
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
	int player_suggestion, weapon_suggestion;
	string player_buffer, weapon_buffer;

	int location = players[player_id].get_location();


	// get player's suggestion for character
	bool success = false;

	while (!success){
		send(
			socket_id, 
			request_player.c_str(), 
			request_player.size(), 
			0
		);

		player_buffer = server.receive_communication(socket_id);
		istringstream(player_buffer) >> player_suggestion;

		// validate input (with the GUI this will probably not be necessary,
		// it will be a conditional based on where the player clicks)
		if ((player_suggestion > 0) && (player_suggestion < 7)){
			success = true;
		}
		else {
			send(
				socket_id,
				invalid_suggestion.c_str(),
				invalid_suggestion.size(),
				0
			);
		}
	}
	
	// get player's suggestion for weapon
	success = false;
	while (!success){
		send(
			socket_id, 
			request_weapon.c_str(), 
			request_weapon.size(), 
			0
		);

		weapon_buffer = server.receive_communication(socket_id);
		istringstream(weapon_buffer) >> weapon_suggestion;


		// validate input
		if ((weapon_suggestion > 6) && (weapon_suggestion < 13)){
			success = true;
		}

		

		else {
			string please = weapon_buffer + " is invalid\n\t";
		
			send(
				socket_id,
				please.c_str(),
				please.size(),
				0
			);
		}
	}
	


	/* TODO: 
		actually move the player there.
		do this by updating the player attribute for location
	*/





	// communicate to all other clients
	string notify_all_clients = "Player " + to_string(player_id) 
		+ " suggests: " + to_string(location) + ", " 
		+ player_buffer + ", " + weapon_buffer + "\n";

	for (int i = 0; i < n_clients; i++){
		if (i != player_id){
			send(
				socket_tracker[i],
				notify_all_clients.c_str(),
				notify_all_clients.size(),
				0
			);
		}
	}

	// also output on the server
	cout << notify_all_clients;


	// finally, loop over all other players
	// when one of them shows any card, break


	return 0;
}




int GamePlay::accuse(int player_id, int* socket_tracker, Server server){
	cout << "not implemented yet" << endl;
	return 0;
}


// generally, at the end of your turn, you have the option to accuse.
// calling this will let you bypass that option
void GamePlay::pass(int player_id, int* socket_tracker, Server server){
	// just communicate to all other clients
	string notify_all_clients = "Player " + to_string(player_id) 
		+ "\'s turn has ended.\n";

	for (int i = 0; i < server.get_n_clients(); i++){
		if (i != player_id){
			send(
				socket_tracker[i],
				notify_all_clients.c_str(),
				notify_all_clients.size(),
				0
			);
		}
	}

	// also output on the server
	cout << notify_all_clients;
	return;
}



string GamePlay::getting_accused(int player_id, int* socket_tracker){
	cout << "not implemented yet" << endl;
	return 0;
}


void GamePlay::set_player_character(int player_id, Player* players){
	players[player_id].set_character(card_map[player_id + 1]);
}
        


// other static attributes
string GamePlay::request_location = "Where would you like to move?\n\t";
string GamePlay::request_player = "Who would you like to suggest?\n\t";
string GamePlay::request_weapon = "With what weapon?\n\t";
string GamePlay::invalid_suggestion = "Invalid suggestion, try again:\n\t";


// it'd be nice tostore players here so we don't have to pass it back and forth
// when executing gameplay logic
// but circular includes and forward declarations don't work with this logic
// void GamePlay::finalize_players(Player *final_players){
// 	players = final_players;
// }

// store and populate maps for relevant functions here
unordered_map<int, string> GamePlay::location_map;
unordered_map<int, string> GamePlay::card_map;

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










