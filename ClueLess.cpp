#include "Server.h"
#include "GamePlay.h"

#include "globals.h"

#define MAX_PENDING_CONN 3


using namespace std;



// struct RNG {
//     int operator() (int n) {
//         return rand() / (1.0 + RAND_MAX) * n;
//     }
// };


/** This drives the remainder of the code in our project.
 * It will instantiate a server object, accept all connections,
 * instantiate all player objects, then enter the main game loop
 * where each player executes their turns until the 
 * winning condition is reached.
 */
int main(int argc, char *argv[]){
	// first get port from runtime args
	if (argc < 3) {
       printf("Usage: %s [port] [n_players]\n", argv[0]);
       exit(1);
    }
	int port = atoi(argv[1]);
	istringstream(argv[2]) >> max_clients;
	// max_clients = 3;
    
	// initialize location and card maps
	// ideally you'd just initialize them when defined, but the c++ compilers 
	// that work with fd_set don't support initializer lists
	populate_location_map();
	populate_card_map();
	populate_bridge();


	// start up server and get connections
	int n_clients, tmp_id;
	

	Server server(port, MAX_PENDING_CONN, STREAM_SIZE, max_clients);
	server.initialize();	// all clients connect here

	n_clients = server.get_n_clients();

	// contains socket ids used to send/receive messages
	int* socket_tracker = server.get_socket_tracker();



	// now initialize players
	// players[n_clients];
	
	bool winner[n_clients];		// to track winning/losing conditions
	bool in_play[n_clients];  
	int players_remaining = n_clients;	// in case everyone strikes out
	string who_are_you;

	for (int i = 0; i < n_clients; i++){
		players[i].set_player_id(i);
		GamePlay::set_player_character(i, &players[i]);

		// who_are_you = "*** You are " + players[i].get_character() + " ***\n";
		// send(socket_tracker[i], who_are_you.c_str(), who_are_you.size(), 0);

		players[i].set_socket_id(socket_tracker[i]);

		in_play[i] = true;
		winner[i] = false;
	}


	


	// Initialize case file and player cards	
	vector<int> player_cards, weapon_cards, location_cards, all_cards;

	for (int card = 1; card <= 21; card++){
		if (card <= 6){
			player_cards.push_back(card);
		}
		else if (card <= 12){
			weapon_cards.push_back(card);
		}
		else {
			location_cards.push_back(card);
		}
	}

	// before shuffling, set seed based on time of execution
	srand(time(0));

	// first shuffle each category individually
	random_shuffle(player_cards.begin(), player_cards.end());
	random_shuffle(weapon_cards.begin(), weapon_cards.end());
	random_shuffle(location_cards.begin(), location_cards.end());

	int cf_player = rand() % 6;
	int cf_weapon = rand() % 6;
	int cf_location = rand() % 9;


	// get one from each category for the case file
	GamePlay::populate_case_file(
		player_cards[cf_player], 
		weapon_cards[cf_weapon], 
		location_cards[cf_location]
	);
	cout << "3 cards reserved in the case file." << endl;


	// remove case file cards from remaining cards
	player_cards.erase(player_cards.begin() + cf_player);
	weapon_cards.erase(weapon_cards.begin() + cf_weapon);
	location_cards.erase(location_cards.begin() + cf_location);


	// bin remaining cards together
	all_cards.insert(all_cards.end(), player_cards.begin(), player_cards.end());
	all_cards.insert(all_cards.end(), weapon_cards.begin(), weapon_cards.end());
	all_cards.insert(all_cards.end(), location_cards.begin(), location_cards.end());


	// and shuffle again
	random_shuffle(all_cards.begin(), all_cards.end());

	// double check size
	if (all_cards.size() != 18){
		cerr << "Unexpected behavior during card shuffling; "
			 << "revisit logic in main()" << endl;
	}


	// finally, shuffle to each player
	for (int i = 0; i < all_cards.size(); i++){
		tmp_id = i % n_clients; 
		players[tmp_id].add_card(all_cards[i]);
	}

	cout << "Remaining 18 cards shuffled out to all players\n" << endl;
	
	

	// show cards to clients once
	for (int i = 0; i < n_clients; i++){
		GamePlay::show_hand(socket_tracker[i], &players[i]);
		if (usleep(10000) == -1) cout << "gameplay: failed to clear buffer" << endl;
	}
	

	
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
		+ card_map[winning_player + 1] + " Wins.";
	cout << loser_message << endl;

	string winner_message = GamePlay::get_win_message();
	sleep(3);

	for (int i = 0; i < n_clients; i++){
		if (winner[i]){
			send(socket_tracker[i], winner_message.c_str(), winner_message.size(), 0);
		}
		else {
			send(socket_tracker[i], loser_message.c_str(), loser_message.size(), 0);
		}
	}





	bool final_quit = false;
	string final_message;

	while (!final_quit){
		final_message = server.receive_any_communication();
		if (final_message.compare("quit") == 0){
			final_quit = true;
		}
	}
	

	server.close_all();

	return 0;
}
