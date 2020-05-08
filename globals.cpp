#include "globals.h"

using namespace std;


bool game_started = false;
int max_clients = 3;

Player players[3];





// store and populate maps for relevant functions here
unordered_map<int, string> location_map;
unordered_map<string, int> reverse_location_map;

unordered_map<int, string> card_map;
unordered_map<string, int> reverse_card_map;

unordered_map< int, vector<int> > valid_move_map;
unordered_map<int, int> bridge;


// used to communicate between client/server
string show_hand_str = "show hand";
string navigate_str = "navigate";
string suggest_str = "suggest";
string accuse_str = "accuse";
string pass_str = "pass";
string stay_str = "stay";
string empty_space = "empty space";



// ideally this would be implemented as a graph, 
// with 4 possible directions to go (e.g. node.left, node.right, etc).
// or we could design separate study and hallway types, and use those
// (e.g. a hallway could contain attributes for which rooms they're 
// connected to, and similarly with the rooms)
// but using a map is easier

// also ideally we would directly initialize it with a list,
// but the compiler that works with fd_set does not support that
void populate_location_map(){
	valid_move_map[1].push_back(2);
	valid_move_map[1].push_back(6);
	valid_move_map[1].push_back(21);	// secret passageway

	valid_move_map[2].push_back(1);
	valid_move_map[2].push_back(3);
	
	valid_move_map[3].push_back(2);
	valid_move_map[3].push_back(4);
	valid_move_map[3].push_back(7);

	valid_move_map[4].push_back(3);
	valid_move_map[4].push_back(5);

	valid_move_map[5].push_back(4);
	valid_move_map[5].push_back(8);
	valid_move_map[5].push_back(17);	// secret passageway

	valid_move_map[6].push_back(1);
	valid_move_map[6].push_back(9);

	valid_move_map[7].push_back(3);
	valid_move_map[7].push_back(11);

	valid_move_map[8].push_back(5);
	valid_move_map[8].push_back(13);

	valid_move_map[9].push_back(6);
	valid_move_map[9].push_back(10);
	valid_move_map[9].push_back(14);

	valid_move_map[10].push_back(9);
	valid_move_map[10].push_back(11);

	valid_move_map[11].push_back(10);
	valid_move_map[11].push_back(12);
	valid_move_map[11].push_back(7);
	valid_move_map[11].push_back(15);

	valid_move_map[12].push_back(11);
	valid_move_map[12].push_back(13);

	valid_move_map[13].push_back(12);
	valid_move_map[13].push_back(8);
	valid_move_map[13].push_back(16);

	valid_move_map[14].push_back(9);
	valid_move_map[14].push_back(17);
	
	valid_move_map[15].push_back(11);
	valid_move_map[15].push_back(19);

	valid_move_map[16].push_back(13);
	valid_move_map[16].push_back(21);

	valid_move_map[17].push_back(14);
	valid_move_map[17].push_back(18);
	valid_move_map[17].push_back(5);	// secret passageway

	valid_move_map[18].push_back(17);
	valid_move_map[18].push_back(19);

	valid_move_map[19].push_back(18);
	valid_move_map[19].push_back(20);
	valid_move_map[19].push_back(15);

	valid_move_map[20].push_back(19);
	valid_move_map[20].push_back(21);
	
	valid_move_map[21].push_back(20);
	valid_move_map[21].push_back(16);
	valid_move_map[21].push_back(1);	// secret passageway


	// starting blocks
	valid_move_map[-1].push_back(4);
	valid_move_map[-2].push_back(6);
	valid_move_map[-3].push_back(14);
	valid_move_map[-4].push_back(18);
	valid_move_map[-5].push_back(20);
	valid_move_map[-6].push_back(8);




	// unordered_map<int, string> locations;
	location_map[1] = "Study";
	location_map[2] = "Hallway (Study, Hall)";
	location_map[3] = "Hall";
	location_map[4] = "Hallway (Hall, Lounge)";
	location_map[5] = "Lounge";
	location_map[6] = "Hallway (Study, Library)";
	location_map[7] = "Hallway (Hall, Billiard Room)";
	location_map[8] = "Hallway (Lounge, Dining)";
	location_map[9] = "Library";
	location_map[10] = "Hallway (Library, Billiard Room)";
	location_map[11] = "Billiard Room";
	location_map[12] = "Hallway (Billiard Room, Dining)";
	location_map[13] = "Dining Room";
	location_map[14] = "Hallway (Library, Conservatory)";
	location_map[15] = "Hallway (Billiard Room, Ballroom)";
	location_map[16] = "Hallway (Dining, Kitchen)";
	location_map[17] = "Conservatory";
	location_map[18] = "Hallway (Conservatory, Ballroom)";
	location_map[19] = "Ballroom";
	location_map[20] = "Hallway (Ballroom, Kitchen)";
	location_map[21] = "Kitchen";

	location_map[-1] = "Starting Block";
	location_map[-2] = "Starting Block";
	location_map[-3] = "Starting Block";
	location_map[-4] = "Starting Block";
	location_map[-5] = "Starting Block";
	location_map[-6] = "Starting Block";

	location_map[30] = show_hand_str;
	location_map[31] = navigate_str;
	location_map[32] = suggest_str;
	location_map[33] = accuse_str;
	location_map[34] = pass_str;
	location_map[35] = stay_str;
	location_map[36] = empty_space;


	// this is horrible, but unfortunately map iteration only 
	// works on compilers that don't work with fd_set
	reverse_location_map["Study"] = 1;
	reverse_location_map["Hallway (Study, Hall)"] = 2;
	reverse_location_map["Hall"] = 3;
	reverse_location_map["Hallway (Hall, Lounge)"] = 4;
	reverse_location_map["Lounge"] = 5;
	reverse_location_map["Hallway (Study, Library)"] = 6;
	reverse_location_map["Hallway (Hall, Billiard Room)"] = 7;
	reverse_location_map["Hallway (Lounge, Dining)"] = 8;
	reverse_location_map["Library"] = 9;
	reverse_location_map["Hallway (Library, Billiard Room)"] = 10;
	reverse_location_map["Billiard Room"] = 11;
	reverse_location_map["Hallway (Billiard Room, Dining)"] = 12;
	reverse_location_map["Dining Room"] = 13;
	reverse_location_map["Hallway (Library, Conservatory)"] = 14;
	reverse_location_map["Hallway (Billiard Room, Ballroom)"] = 15;
	reverse_location_map["Hallway (Dining, Kitchen)"] = 16;
	reverse_location_map["Conservatory"] = 17;
	reverse_location_map["Hallway (Conservatory, Ballroom)"] = 18;
	reverse_location_map["Ballroom"] = 19;
	reverse_location_map["Hallway (Ballroom, Kitchen)"] = 20;
	reverse_location_map["Kitchen"] = 21;

    
}

void populate_card_map(){
	// using 0 indexed would play better with player_id
	// but some string - int conversion methods return 0 when invalid
	card_map[1] = "Miss Scarlet";
	card_map[2] = "Prof. Plum";	
	card_map[3] = "Mrs. Peacock";
	card_map[4] = "Mr. Green";
	card_map[5] = "Mrs. White";
	card_map[6] = "Col. Mustard";

	card_map[7] = "Rope";
	card_map[8] = "Lead Pipe";
	card_map[9] = "Knife";
	card_map[10] = "Wrench";
	card_map[11] = "Candlestick";
	card_map[12] = "Revolver";

	card_map[13] = "Study";
	card_map[14] = "Hall";
	card_map[15] = "Lounge";
	card_map[16] = "Library";
	card_map[17] = "Billiard Room";
	card_map[18] = "Dining Room";
	card_map[19] = "Conservatory";
	card_map[20] = "Ballroom";
	card_map[21] = "Kitchen";

	reverse_card_map["Miss Scarlet"] = 1;
	reverse_card_map["Prof. Plum"] = 2;
	reverse_card_map["Mrs. Peacock"] = 3;
	reverse_card_map["Mr. Green"] = 4;
	reverse_card_map["Mrs. White"] = 5;
	reverse_card_map["Col. Mustard"] = 6;

	reverse_card_map["Rope"] = 7;
	reverse_card_map["Lead Pipe"] = 8;
	reverse_card_map["Knife"] = 9;
	reverse_card_map["Wrench"] = 10;
	reverse_card_map["Candlestick"] = 11;
	reverse_card_map["Revolver"] = 12;

	reverse_card_map["Study"] = 13;
	reverse_card_map["Hall"] = 14;
	reverse_card_map["Lounge"] = 15;
	reverse_card_map["Library"] = 16;
	reverse_card_map["Billiard Room"] = 17;
	reverse_card_map["Dining Room"] = 18;
	reverse_card_map["Conservatory"] = 19;
	reverse_card_map["Ballroom"] = 20;
	reverse_card_map["Kitchen"] = 21;



}

void populate_bridge(){
	// bridge from location rooms to card rooms
	bridge[1] = 13;
	bridge[3] = 14;
	bridge[5] = 15;
	bridge[9] = 16;
	bridge[11] = 17;
	bridge[13] = 18;
	bridge[17] = 19;
	bridge[19] = 20;
	bridge[21] = 21;
}


bool is_semicolon(char c){
	return c == ';';
}



// strings used for cli communication
string request_location = "Where would you like to move?";
string request_player = "Who do you think committed the crime?";
string accuse_location = "In what room?";
string request_weapon = "With what weapon?";

// all used in gameplay execute_turn
string start_str = "Your turn.";
string navigate_stay_str = "Do you want to navigate or stay?";
string suggest_accuse_str = "Do you want to suggest, accuse, or pass?";
string accuse_pass_str = "Do you want to accuse or pass?";
string navigate_accuse_str = "Do you want to navigate or accuse?";
string force_stay_str = "You have no valid moves";
string check_hand = "Check hand? [y]/[n]: ";
string check_state = "Check player locations? [y]/[n]: ";
string turn_end_str = "Turn ended.";

string invalid_input = "Invalid input.";
string invalid_str = "invalid";

string force_move = "Only one choice for navigation.";

string no_show_individual = "You do not have any of the suggested cards.";
string nobody_showed = "Nobody showed.";
string end_turn_str = "Ending.";
string deactivate_str = "Incorrect; deactivating.";
string winner_message = "You win!";

// for validating inputs (probably won't need this with a GUI)
// these terminating strings are not great, but it's impossible to
// determine the size of an array using its pointer
string yes_no[] = {"y", "n", ""};


string navigate_stay[] = {navigate_str, stay_str, ""};
string navigate_accuse[] = {navigate_str, accuse_str, ""};
string suggest_accuse[] = {suggest_str, accuse_str, pass_str, ""};
string accuse_pass[] = {accuse_str, pass_str, ""};



string scarlet = "Miss Scarlet";
string plum = "Prof. Plum";
string peacock = "Mrs. Peacock";
string green = "Mr. Green";
string white = "Mrs. White";
string mustard = "Col. Mustard";
string rope = "Rope";
string leadpipe = "Lead Pipe";
string knife = "Knife";
string wrench = "Wrench";
string candlestick = "Candlestick";
string revolver = "Revolver";
string study = "Study";
string hall = "Hall";
string lounge = "Lounge";
string library = "Library";
string billiard = "Billiard Room";
string dining = "Dining Room";
string conservatory = "Conservatory";
string ballroom = "Ballroom";
string kitchen = "Kitchen";


int all_rooms[9] = {1, 3, 5, 9, 11, 13, 17, 19, 21};	// locations






