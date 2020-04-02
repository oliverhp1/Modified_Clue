#include "GamePlay.h"

using namespace std;



// these are all the actions that a player might take on their turn.

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



