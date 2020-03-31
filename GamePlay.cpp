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



static void navigate(int player_id, int socket_id, int* socket_tracker){
	int socket_id = socket_tracker[player_id];

	// ask what room player wants to move to
	string request_location = "Where would you like to move?";

	send(
		socket_id, 
		message.c_str(), 
		message.size(), 
		0
	);

	// get answer
	string location_buffer = receive_communication(socket_id);


	/* TODO: actual logic here

	 first check that the choice was valid. actually ideally, we'd print all valid choices beforehand
	 then, actually move the player there.
	*/


	// communicate to all other clients
	char* notify_all_clients = "Player " + to_string(player_id) 
		+ " is moving to " + location_buffer;

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




	cout << notify_all_clients << endl;
	return ;

}
static int suggest(int, int);
static int accuse(int, int);




static string getting_accused(int, int);

