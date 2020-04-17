#pragma once

#include "Server.h"
#include "Player.h"


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

        // this will drive the structure of a turn (by calling below methods)
        static void execute_turn(int, Server, Player*);

        // each of these will use player_id and socket_id
        // corresponding to which player is doing the action
    	static void navigate(int, int*, Server);
    	static int suggest(int, int*, Server, Player*);
        static void accuse(int, int*, Server, Player*);
        static void pass(int, int*, Server);


        // helper methods
        static int get_bounded_input(
        	int socket_id, Server server, string message, int lo, int hi
        );
    	

        // set player attributes
        static void set_player_character(int, Player*);

        // maintain case file
        static int case_file[];
        static string case_file_string;
        static void populate_case_file(int, int, int);

        // set up maps for relevant functionality
        static unordered_map<int, string> location_map;
        static unordered_map<int, string> card_map;

        static void populate_location_map();
        static void populate_card_map();

        // helper strings for communication
        static string request_location;
        static string request_player;
        static string request_weapon;
        static string accuse_location;
        static string invalid_input;
        static string wrong_accusation;



};
