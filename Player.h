#include "Server.h"
#include "GamePlay.h"


using namespace std;


/*     TODO
	This is our player class.
	it contains attributes for...
	it has a method execute_turn, which structures the turn of a player
*/
class Player {
	public: 
		
    	Player();

    	void set_player_id(int);
    	void set_socket_id(int);
    	
    	bool is_in_play();	// track accusation logic
    	bool did_win();

    	void execute_turn(Server);
    	string show_hand();	  // do at any time. Also print at the beginning

    	// string getting_accused();	// when another player accuses this player


    private: 
    	int player_id, socket_id;
        int location;   // corresponds to the 'location' map in globals
    	bool in_play, win;	// if player has made accusation, update these

    	// since we are allowing any number of players, hands can
    	// vary in size. so a vector is preferable to an array
    	vector<int> hand; 	

		// WE WILL NEED A GLOBAL SOMEWHERE, WITH A MAP OF INTS TO ALL THE CARDS
			// either in gameplay, or make a global file.  




};
