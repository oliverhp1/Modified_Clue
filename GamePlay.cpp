#include "GamePlay.h"

using namespace std;


/* on a player's turn they have several options:
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



void GamePlay::navigate(int player_id, int* socket_tracker, Server server){
	int socket_id = socket_tracker[player_id];
	int n_clients = server.get_n_clients();

	// ask what room player wants to move to
	string request_location = "Where would you like to move?\n\t";

	send(
		socket_id, 
		request_location.c_str(), 
		request_location.size(), 
		0
	);

	// get answer
	string location_buffer = server.receive_communication(socket_id);


	/* TODO: actual logic here

	 first check that the choice was valid. actually ideally, we'd print all valid choices beforehand
	 then, actually move the player there.
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



