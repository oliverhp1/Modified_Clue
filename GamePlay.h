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
    	static void navigate(int, vector<string>, int*, Server, Player*);
    	static int suggest(int, int*, Server, Player*);
        static void accuse(int, int*, Server, Player*);
        static void pass(int, int*, Server);


        // helper methods
        static string get_bounded_input(
        	int, Server, string, int, int
        );

        // gets valid string from outputs, then executes corresponding move
    	static void bounded_move(
    		int, int*, Server, string, string[], Player*
		);

		static string get_valid_input(int, Server, string, string[]);
		static string get_contained_input(int, Server, string, vector<string>);
		static void call_valid_move(int, int*, vector<string>, Server, string, Player*);


		static void show_hand(int, Player*);
		static void print_state(int, int, Player*);

		


        // set player attributes
        static void set_player_character(int, Player*);

        // maintain case file
        static int case_file[];
        static string case_file_string;
        static void populate_case_file(int, int, int);


        // for determining whether player is in a room
        static int all_rooms[];
        static string yes_no[];
        static bool in_room(Player*);


        // set up maps for relevant functionality
        static unordered_map<int, string> location_map, card_map;
        static unordered_map<string, int> reverse_location_map, reverse_card_map;
    	
        static unordered_map<int, int> bridge;
        static unordered_map< int, vector<int> > valid_move_map;
        

        static void populate_location_map();
        static void populate_card_map();
        static void populate_bridge();


        // helper strings for communication
        static string request_location, request_player, request_weapon;
        static string accuse_location, wrong_accusation;

        static string navigate_stay_str, suggest_accuse_str, accuse_pass_str;
		static string start_str, navigate_accuse_str, force_stay_str;
		static string check_hand, check_state;
		
		
		static string invalid_input, no_show_individual;

		static string navigate_stay[], suggest_accuse[], accuse_pass[], navigate_accuse[];
        



};
