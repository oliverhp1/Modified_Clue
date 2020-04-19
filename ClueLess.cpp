#include "Server.h"
#include "Player.h"
#include "GamePlay.h"

// TODO: pass this as runtime arg instead of hardcoding
#define PORT 10005
#define MAX_PENDING_CONN 3


using namespace std;



/** This drives the remainder of the code in our project.
 * It will instantiate a server object, accept all connections,
 * instantiate all player objects, then enter the main game loop
 * where each player executes their turns until the 
 * winning condition is reached.
 */
int main(int argc, char *argv[]){
	// initialize location and card maps
	// ideally you'd just initialize them when defined, but the c++ compilers 
	// that work with fd_set don't support initializer lists
	GamePlay::populate_location_map();
	GamePlay::populate_card_map();
	GamePlay::populate_bridge();
	Player::initialize_map();



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
	string who_are_you;

	for (int i = 0; i < n_clients; i++){
		players[i].set_player_id(i);	// also sets characters
		GamePlay::set_player_character(i, &players[i]);

		who_are_you = "*** You are " + players[i].get_character() + " ***\n";
		send(socket_tracker[i], who_are_you.c_str(), who_are_you.size(), 0);

		players[i].set_socket_id(socket_tracker[i]);

		in_play[i] = true;
		winner[i] = false;
	}


	




	// TODO: INITIALIZE PLAYERS' CARDS AS WELL AS CASE FILE CARDS
	/*  see the card_map defined at the bottom of GamePlay.cpp
		you'll want one of each of (1 through 6), (7-12), and (13-21) in the case file
		something like this:

	random sample 6 numbers from numbers 1-6, without replacement (this will serve to shuffle # 1-6)
		also do 7-12 and 13-21 separately
	stick the first from each category in case file
		use GamePlay::populate_case_file(int card1, int card2, int card3) to put cards in the case file
	then shuffle remaining numbers together
	then, for all remaining cards, players[i].add_card(card)
		where i is player_id.  
		note that we'll want to support a variable number of players,
		so their hand lengths may not be the same.

	After this, you'll want to send a message to each player, telling them what cards they drew.
	You can copy the code from above (the code with string "who_are_you")
	*/


	// PLACEHOLDER: REMOVE THIS WHEN THE ABOVE IS IMPLEMENTED
		// note that you can put case file cards in any order. the populate_case_file method will sort it automatically
	GamePlay::populate_case_file(15, 7, 1);

	players[0].add_card(2);
	players[0].add_card(8);
	players[0].add_card(16);

	players[1].add_card(3);
	players[1].add_card(9);
	players[1].add_card(17);

	if (n_clients > 2){
		players[2].add_card(4);
		players[2].add_card(10);
		players[2].add_card(18);
	}




	cout << "3 cards reserved in the case file." << endl;
	cout << "Remaining 18 cards shuffled out to all players\n" << endl;


	// show cards to players once
	// using method void GamePlay::show_hand(int socket_id, Player* player)

	// set players as a static attribute of GamePlay
	// GamePlay::finalize_players(players);

	// finally, this is the main game loop
	bool game_active = true;
	bool player_won = false;

	while (game_active){
		for (int i = 0; i < n_clients; i++){
			if (in_play[i]){
				// last one standing wins
				// check before executing turn
				if (players_remaining == 1){
					for (int j = 0; j < n_clients; j++){
						winner[i] = in_play[i];
					}
					
					game_active = false;
					break;
				}

				// most turn logic encapsulated here
				GamePlay::execute_turn(i, server, players);


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
	string loser_message = "Game Over: " 
		+ GamePlay::card_map[winning_player + 1] + " Wins.\r\n";
	string winner_message = "You win!\r\n";

	cout << loser_message << endl;

	for (int i = 0; i < n_clients; i++){
		if (winner[i]){
			send(socket_tracker[i], winner_message.c_str(), winner_message.size(), 0);
		}
		else {
			send(socket_tracker[i], loser_message.c_str(), loser_message.size(), 0);
		}
	}
	

	server.close_all();

	return 0;
}
