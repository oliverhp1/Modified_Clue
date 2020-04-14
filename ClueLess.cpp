#include "Player.h"

// TODO: pass this as runtime arg instead of hardcoding
#define PORT 10002
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


	// initialize location and card maps
	// ideally you'd just initialize them when defined, but the c++ compilers 
	// that work with fd_set don't support initializer lists
	GamePlay::populate_location_map();
	GamePlay::populate_card_map();
	cout << GamePlay::location_map[1] << endl;
	cout << GamePlay::card_map[21] << endl;


	// TODO: INITIALIZE PLAYERS' CARDS AS WELL AS CASE FILE CARDS
	/* something like this:

	int case_file[3];
	random sample numbers 1-21, without replacement
	stick first 3 in case file
	then for 4:21, players[i].add_card(card)
		where i is player_id.  
		you'll need to make an "add_card" method for the Player class, 
		that adds cards to their "hand" attribute (a vector- look up how to append to vector on google).
		note that we'll want to support a variable number of players,
		so their hand lengths may not be the same.
	*/

	cout << "3 cards reserved in the case file." << endl;
	cout << "Remaining 18 cards shuffled out to all players\n" << endl;

	// show cards to players once


	// finally, this is the main game loop
	bool game_active = true;
	bool player_won = false;

	while (game_active){
		for (int i = 0; i < n_clients; i++){
			if (in_play[i]){
				players[i].execute_turn(server);
					// build gameplay logic into here, or outside of here. either way, need a way to communicate between players.
				// AND WE NEED TO BROADCAST EVERYTHING AFTER ANYTHING HAPPENS


				in_play[i] = players[i].is_in_play();
				winner[i] = players[i].did_win();



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


	// check for winner
	int winning_player;
	for (int i = 0; i < n_clients; i++){
		if (winner[i]){
			winning_player = i;
			break;
		}
	}

	// broadcast to all 
	string loser_message = "Game Over: Player " + to_string(winning_player) + " Wins.\r\n";
	string winner_message = "You win!\r\n";

	cout << loser_message << endl;

	for (int i = 0; i < n_clients; i++){
		if (winner[i]){
			send(
				socket_tracker[i], 
				winner_message.c_str(), 
				winner_message.size(), 
				0
			);
		}
		else {
			send(
				socket_tracker[i], 
				loser_message.c_str(), 
				loser_message.size(), 
				0
			);
		}
	}
	

	server.close_all();

	return 0;
}
