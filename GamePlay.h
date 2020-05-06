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
		static bool get_matching_input(int, Server, string);

		static void call_valid_move(int, int*, vector<string>, Server, string, Player*);


		static void show_hand(int, Player*);
		static void print_state(int, int, Player*);

		


        // set player attributes
        static void set_player_character(int, Player*);

        // maintain case file
        static int case_file[];
        static string case_file_string, wrong_accusation;
        static void populate_case_file(int, int, int);


        // for determining whether player is in a room
        static bool in_room(Player*);


        



};
