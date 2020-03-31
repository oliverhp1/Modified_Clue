#include "Player.h"
#include "GamePlay.h"

#define PORT 10000	// TODO: pass this as runtime arg instead of hardcoding
#define MAX_PENDING_CONN 3


using namespace std;


int main(int argc, char *argv[]){
	// start up server and get connections
	int n_clients;

	Server server(PORT, MAX_CLIENTS, MAX_PENDING_CONN, STREAM_SIZE);
	server.initialize();

	n_clients = server.get_n_clients();

	// contains socket ids used to send/receive messages
	int* socket_tracker = server.get_socket_tracker();



	// now initialize players
	Player players[n_clients];
	
	bool winner[n_clients];		// to track winning/losing conditions
	bool in_play[n_clients];  
	int players_remaining = n_clients;	// in case everyone strikes out

	for (int i = 0; i < n_clients; i++){
		players[i].set_player_id(i);
		players[i].set_socket_id(socket_tracker[i]);

		in_play[i] = true;
		winner[i] = false;
	}


	// TODO: INITIALIZE PLAYERS' CARDS AS WELL AS CASE FILE CARDS
	// ALSO NEED TO MAKE GLOBAL MAP FOR WHAT CARD CORRESPONDS TO WHAT
	// 		probably makes sense to make that map in GamePlay.h, or a global file.
	/* something like this:

	int case_file[3];
	random sample numbers 1-21, without replacement
	stick first 3 in case file
	then for 4:21, player[i].add_card(card)
		where i is player_id.  we can start i from 0 and use the mod at each increment, 
		or reset to 0 whenever we hit max_clients
	*/


	// finally, this is the main game loop
	bool game_active = true;
	bool player_won = false;

	while (game_active){
		for (int i = 0; i < n_clients; i++){
			if (in_play[i]){
				cout << players[i].execute_turn(server) << endl;
					// build gameplay logic into here, or outside of here. either way, need a way to communicate between players.
				// AND WE NEED TO BROADCAST EVERYTHING AFTER ANYTHING HAPPENS


				in_play[i] = players[i].is_in_play();
				winner[i] = players[i].did_win();

				winner[i] = true;



				// check all winning conditions
				if (winner[i]){
					game_active = false;
					break;
				}
				if (!in_play[i]){
					// changed from in play to not in play
					players_remaining--;
				}

				if (players_remaining == 1){
					// last one standing wins
					// check right after anybody is removed
					for (int j = 0; j < n_clients; j++){
						winner[i] = in_play[i];
					}
					
					game_active = false;
					break;
				}



			}
		}

		// if we get here, every player has gone once
		// we want to keep going if game is still active
		
	}
	

	// server.close_all();

	return 0;
}
