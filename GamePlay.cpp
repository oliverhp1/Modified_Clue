#include "GamePlay.h"

using namespace std;



/** These are all the actions that a player might take on their turn.
 * They are implemented as static methods so we don't have to worry about
 * keeping track of a separate object instance and calling from it.
 * Methods correspond to general Clue gameplay:
 *	navigate
 *	suggest
 *	accuse
 *	pass
 */

void GamePlay::navigate(int player_id, int* socket_tracker, Server server){
	int socket_id = socket_tracker[player_id];
	int n_clients = server.get_n_clients();


	// TODO: FIGURE OUT VALID CHOICES TO MOVE TO BEFOREHAND. 
	// please do this in the Player.cpp file, under execute_turn() method if possible.
	// you'll probably want to make an attribute (e.g. static attribute in GamePlay) tracking where all players locations are.
	// for the logic, refer to the diagram and rules in the file describing our class project.
	// you can use location_map.  store a player's current location in an attribute, 
	// e.g. player::location (preferably, make it a private attribute and use setter and getter methods),
	// and then use that and the location_map to determine valid places to move.
	// then stick it in "request_location" string below.
	// syntax for location map is just: location_map[1] will return "Study", and so on. (see bottom of this file)
	// 
	// e.g. "Would you like to move to the " + option1 + " or"... etc

	string request_location = "Where would you like to move?\n\t";

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



int GamePlay::suggest(int player_id, int* socket_tracker, Server server){
	cout << "not implemented yet" << endl;
	return 0;
}

int GamePlay::accuse(int player_id, int* socket_tracker, Server server){
	cout << "not implemented yet" << endl;
	return 0;
}

int GamePlay::pass(int player_id, int* socket_tracker, Server server){
	cout << "not implemented yet" << endl;
	return 0;
}



string GamePlay::getting_accused(int player_id, int* socket_tracker){
	cout << "not implemented yet" << endl;
	return 0;
}



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









