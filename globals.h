#pragma once

#include "Player.h"


#include <iostream>
#include <unordered_map>

#include <stdio.h>  
#include <string.h>
#include <stdlib.h>  
#include <vector> 
#include <sstream>

#include <errno.h>  
#include <unistd.h>
#include <arpa/inet.h>   

#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/time.h>

#include <netinet/in.h>



using namespace std;


extern bool game_started;

extern Player players[];
extern int max_clients;


// set up maps for relevant functionality
extern unordered_map<int, string> location_map;
extern unordered_map<string, int> reverse_location_map;

extern unordered_map<int, string> card_map;
extern unordered_map<string, int> reverse_card_map;

extern unordered_map< int, vector<int> > valid_move_map;
extern unordered_map<int, int> bridge;


extern void populate_location_map();
extern void populate_card_map();
extern void populate_bridge();

extern int all_rooms[9];
extern string yes_no[];


// helper strings for communication
extern string request_location, request_player, request_weapon;
extern string accuse_location;

extern string navigate_stay_str, suggest_accuse_str, accuse_pass_str;
extern string start_str, navigate_accuse_str, force_stay_str, force_move;
extern string check_hand, check_state, turn_end_str, end_turn_str;


extern string invalid_input, no_show_individual, nobody_showed;

extern string navigate_stay[], suggest_accuse[], accuse_pass[], navigate_accuse[];

// this is not necessary but is cleaner than hardcoding
extern string show_hand_str, navigate_str, suggest_str, accuse_str, pass_str, stay_str, invalid_str;
extern string scarlet, plum, peacock, green, white, mustard;
extern string rope, leadpipe, knife, wrench, candle, revolver;
extern string study, hall, lounge, library, billiard, dining, conservatory, ballroom, kitchen, empty_space;

extern bool is_semicolon(char c);







