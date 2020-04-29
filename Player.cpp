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



