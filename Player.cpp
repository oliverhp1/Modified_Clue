#include "Player.h"



Player::Player() {
	this->in_play = true;
	this->win = false;

}








// setter and getter methods

// we can also set player's character here
void Player::set_player_id(int player_id){
	this->player_id = player_id;

	cout << "Player " << player_id << " initialized" << endl;
}

void Player::set_socket_id(int socket_id){
	this->socket_id = socket_id;
}

void Player::set_character(string character){
	this->character = character;
}

void Player::set_location(int location){
	this->location = location;
}

int Player::get_location(){
	return this->location;
}
string Player::get_character(){
	return this->character;
}

bool Player::is_in_play(){
	return in_play;
}

bool Player::did_win(){
	return win;
}

void Player::set_win(){
	this->win = true;
}
