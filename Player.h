#pragma once

#include "Server.h"
// #include "GamePlay.h"

using namespace std;


/*     TODO
	This is our player class.
	it contains attributes for...
	it has a method execute_turn, which structures the turn of a player
*/
class Player {
	public: 
		
    	Player();

        // game logic methods
        string show_hand();   // do at any time. Also print at the beginning

        void add_card(int);

        // setter and getter methods
    	void set_player_id(int);
    	void set_socket_id(int);
        void set_location(int);
        void set_character(string);

        bool get_just_suggested();
        void set_just_suggested(bool);

        int get_location();
        string get_character();
        vector<int> get_hand();
        bool hand_contains(int);

    	
    	// track accusation logic (winning/active)
    	bool is_in_play();	
    	bool did_win();
        void set_win();
        void deactivate();




    private: 
    	int player_id, socket_id;
        int location;   // corresponds to the 'location' map in gameplay
    	bool in_play, win;	// if player has made accusation, update these
    	bool just_suggested;	// if yes, they're always allowed to suggest

        string character;

    	// since we are allowing any number of players, hands can
    	// vary in size. so a vector is preferable to an array
    	vector<int> hand;

    	static unordered_map<int, int> initial_location_map;





};
