#include "Player.h"



Player::Player() {
	this->in_play = true;
	this->win = false;

}



int Player::execute_turn(){
	string message = "Your turn.  Navigate, suggest, or accuse?\r\n";

	send(
		socket_id, 
		message.c_str(), 
		message.size(), 
		0
	);
	return player_id;
}

// string Player::getting_accused(){
// 	return "sup";
// }


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


