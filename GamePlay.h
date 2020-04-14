#include "Server.h"

using namespace std;


/*
 * This class is a helper class that carries out all the gameplay functionality
 * 
 * In particular, all the logic from player moves and actions is included,
 * as well as notifying all players of all relevant changes.
 *
 * For simplicity, we will just leave these as static methods
 * rather than making a separate object instance and handling that.
 */
class GamePlay {
	public:     
    	// GamePlay();

        // each of these will get player_id and socket_id
        // corresponding to which player is doing the action
    	static void navigate(int, int*, Server);
    	static int suggest(int, int*, Server);
        static int accuse(int, int*, Server);
        static int pass(int, int*, Server);



    	
    	static string getting_accused(int, int*);

        static unordered_map<int, string> location_map;
        static unordered_map<int, string> card_map;

        static void populate_location_map();
        static void populate_card_map();



};