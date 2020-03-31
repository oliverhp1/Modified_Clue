#include "Player.h"



Player::Player() {
	this->in_play = true;
	this->win = false;

}




/* 
	this method is the overall driver of a player's turn.
	it will call static methods from GamePlay depending on what action
	the player chooses to take.
	messages will also be pushed out to all other players.
*/
void Player::execute_turn(Server server){
	// ask what player wants to do
	string message = "Your turn.  \n"
		"Do you want to navigate, suggest, accuse, or pass?\r\n\t";

	string invalid_choice = "Invalid choice. Choose again: \n\t";


	// should add an option here to check hand again

	int* socket_tracker = server.get_socket_tracker();
	
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
			GamePlay::navigate(player_id, socket_tracker, server);
		}
		else if (action_buffer.compare("suggest") == 0){
			GamePlay::suggest(player_id, socket_tracker, server);
		}
		else if (action_buffer.compare("accuse") == 0){
			GamePlay::accuse(player_id, socket_tracker, server);
		}
		else if (action_buffer.compare("pass") == 0){
			GamePlay::pass(player_id, socket_tracker, server);
		}
		else if (action_buffer.compare("I win") == 0){
			win = true;
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








// setter and getter methods

void Player::set_player_id(int player_id){
	this->player_id = player_id;

	cout << "Player " << player_id << " initialized" << endl;
}

void Player::set_socket_id(int socket_id){
	this->socket_id = socket_id;
}


bool Player::is_in_play(){
	return in_play;
}

bool Player::did_win(){
	return win;
}


