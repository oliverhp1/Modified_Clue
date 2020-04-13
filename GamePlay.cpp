#include "GamePlay.h"

using namespace std;



// these are all the actions that a player might take on their turn.

void GamePlay::navigate(int player_id, int* socket_tracker, Server server){
	int socket_id = socket_tracker[player_id];
	int n_clients = server.get_n_clients();

	// ask what room player wants to move to

	// TODO: FIGURE OUT VALID CHOICES TO MOVE TO BEFOREHAND
	// refer to the diagram and rules in the problem statement
	// then stick it in this string
	// e.g. "Would you like to move to the " + option1 + " or"... etc
	string request_location = "Where would you like to move?\n\t";

	cout << location_map[1] << endl;


	send(
		socket_id, 
		request_location.c_str(), 
		request_location.size(), 
		0
	);

	// get answer
	string location_buffer = server.receive_communication(socket_id);


	/* TODO: actual logic here

		first check that the choice was valid.
		then, actually move the player there.
		do this by updating the player attribute for location
		will probably want to make a private method "move_player(int location)" to do this
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


unordered_map<int, string> GamePlay::location_map;

void GamePlay::populate_location_map(){
	// unordered_map<int, string> locations;

	location_map[1] = "Study";
	// return locations;
}
// static unordered_map<int, string> generate_card_map();
