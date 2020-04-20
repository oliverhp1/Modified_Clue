#include "Player.h"



Player::Player() {
	this->in_play = true;
	this->win = false;
	this->just_suggested = false;
	
}




// sort while adding. not necessary but nice to have
// (also not that efficient, but inconsequential in this use case)
void Player::add_card(int card){
	hand.push_back(card);
    sort(hand.begin(), hand.end());

	return;
}


// map: player_id -> initial location
// need a separate method to initialize because of compiler version conflicts
unordered_map<int, int> Player::initial_location_map;

// void Player::initialize_map(){			// TESTING: WILL NEED TO FILL IN WITH STARTING BLOCKS LATER

// 	initial_location_map[0] = 1;	// p0, scarlet, (hall/lounge)
// 	initial_location_map[1] = 2;	// p1, plum, (study/library)
// 	initial_location_map[2] = 6;	// p2, peacock, (library/conservatory)
// 	initial_location_map[3] = 18;	// p3, green, (conservatory/ballroom)
// 	initial_location_map[4] = 20;	// p4, white, (ballroom/kitchen)
// 	initial_location_map[5] = 8;	// p5, mustard, (lounge/dining)


// }


// setter and getter methods

// we can also set player's character here
void Player::set_player_id(int player_id){
	this->player_id = player_id;

	// initial location depends only on player_id
	this->set_location(-1 * player_id - 1);

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

void Player::set_just_suggested(bool just_suggested){
	this->just_suggested = just_suggested;
}

bool Player::get_just_suggested(){
	return this->just_suggested;
}

int Player::get_location(){
	return this->location;
}

string Player::get_character(){
	return this->character;
}

vector<int> Player::get_hand(){
	return this->hand;
}

bool Player::hand_contains(int card){
	for (int i = 0; i < hand.size(); i++){
		if (hand[i] == card){
			return true;
		}
	}
	return false;
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



