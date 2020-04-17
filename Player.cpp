#include "Player.h"



Player::Player() {
	this->in_play = true;
	this->win = false;

}








// map: player_id -> initial location
// need a separate method to initialize because of compiler version conflicts
unordered_map<int, int> Player::initial_location_map;

void Player::initialize_map(){
	initial_location_map[0] = 4;	// p0, scarlet, (hall/lounge)
	initial_location_map[1] = 6;	// p1, plum, (study/library)
	initial_location_map[2] = 14;	// p2, peacock, (library/conservatory)
	initial_location_map[3] = 18;	// p3, green, (conservatory/ballroom)
	initial_location_map[4] = 20;	// p4, white, (ballroom/kitchen)
	initial_location_map[5] = 8;	// p5, mustard, (lounge/dining)


}


// setter and getter methods

// we can also set player's character here
void Player::set_player_id(int player_id){
	this->player_id = player_id;

	// initial location depends only on player_id
	this->set_location(initial_location_map[player_id]);

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

void Player::deactivate(){
	this->in_play = false;
}



