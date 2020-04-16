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
        // void execute_turn(Server);
        string show_hand();   // do at any time. Also print at the beginning


        // setter and getter methods
    	void set_player_id(int);
    	void set_socket_id(int);
        void set_location(int);
        void set_character(string);

        int get_location();
        string get_character();
    	
    	bool is_in_play();	// track accusation logic
    	bool did_win();
        void set_win();

    	
    	// string getting_accused();	// when another player accuses this player


    private: 
    	int player_id, socket_id;
        int location;   // corresponds to the 'location' map in gameplay
    	bool in_play, win;	// if player has made accusation, update these

        string character;

    	// since we are allowing any number of players, hands can
    	// vary in size. so a vector is preferable to an array
    	vector<int> hand; 	





};
