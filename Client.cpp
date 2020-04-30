#include "Client.h"
#include <netdb.h>

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

using namespace std;






SDL_Texture* load_image(string path, SDL_Renderer* renderer){
	// free();
	SDL_Texture* tmp_texture = NULL;
	SDL_Surface* load_surface = IMG_Load(path.c_str());

	if (load_surface == NULL){
		printf("load %s error: %s\n", path.c_str(), IMG_GetError());
	}
	else{
		tmp_texture = SDL_CreateTextureFromSurface(renderer, load_surface);
		SDL_FreeSurface(load_surface);
	}

	if (tmp_texture == NULL){
		cout << "PLZZZ" << endl;
	}

	// sTexture = tmp_texture;
	return tmp_texture;

}


/*
 * this method returns an int depending on where on the board the mouse is
 * it is used in tandem with a mouse click event
 * to decide what gameplay logic to execute
 * return: int corresponding to board location integers
 * 		-10 corresponds to any other location (no special function)
 * 		ADD OTHER SPECIAL INTS HERE AS THEY COME UP!
 */
int handle_board_mouse(){		
	int mouse_x = 0;
	int mouse_y = 0;		// mouse coordinates
	
	int result = -10;

	SDL_GetMouseState(&mouse_x, &mouse_y);

	// cout << "x, y = " << mouse_x << ", " << mouse_y << endl;	// for debugging purposes

	if ((mouse_x > 0) && (mouse_x < SCREEN_WIDTH / 2) && (mouse_y > 0) && (mouse_y < SCREEN_HEIGHT / 2)){
		// TOP LEFT
		result = 1;
	}
	else if ((mouse_x < SCREEN_WIDTH) && (mouse_x >= SCREEN_WIDTH / 2) && (mouse_y > 0) && (mouse_y < SCREEN_HEIGHT / 2)){
		// TOP RIGHT
		result = 2;
	}
	else if ((mouse_x > 0) && (mouse_x < SCREEN_WIDTH / 2) && (mouse_y < SCREEN_HEIGHT) && (mouse_y > SCREEN_HEIGHT / 2)){
		// BOTTOM LEFT
		result = 3;
	}
	else if ((mouse_x < SCREEN_WIDTH) && (mouse_x > SCREEN_WIDTH / 2) && (mouse_y < SCREEN_HEIGHT) && (mouse_y > SCREEN_HEIGHT / 2)){
		// TOP RIGHT
		result = 4;
	}

	// can use these for ease if we're custom rendering stuff
	// else if ( (mouse_x > TextTexture_R[Main_Instruct].x) && (mouse_x < TextTexture_R[Main_Instruct].x + TextTexture_R[Main_Instruct].w) && (mouse_y > TextTexture_R[Main_Instruct].y) && (mouse_y < TextTexture_R[Main_Instruct].y + TextTexture_R[Main_Instruct].h)){
	// 	overInstruct = true;
	// 	result = 2;
	// }
	// else if ( (mouse_x > TextTexture_R[Main_Quit].x) && (mouse_x < TextTexture_R[Main_Quit].x + TextTexture_R[Main_Quit].w) && (mouse_y > TextTexture_R[Main_Quit].y) && (mouse_y < TextTexture_R[Main_Quit].y + TextTexture_R[Main_Quit].h)){
	// 	overQuit = true;
	// 	result = 3;
	// }

	// can use these to render highlight sections over whatever portion the mouse is over
	// ideally only do that if it's a valid move. but that's not very straightforward
	// SDL_RenderCopyEx(gRenderer, TextTextures[Main_Asteroids], NULL, &TextTexture_R[Main_Asteroids], 0, NULL, SDL_FLIP_NONE);

	// if (!overPlay){
	// 	SDL_RenderCopyEx(gRenderer, TextTextures[Main_Play], NULL, &TextTexture_R[Main_Play], 0, NULL, SDL_FLIP_NONE);
	// }
	// else{
	// 	SDL_RenderCopyEx(gRenderer, TextTextures[Main_Play_H], NULL, &TextTexture_R[Main_Play_H], 0, NULL, SDL_FLIP_NONE);
	// }

	return result;
}




string ping_server(fd_set* server_set, int max_connection, int client_socket, timeval quick){
	string message = "";
	char buffer[STREAM_SIZE];
	int incoming_stream;

	// reset fd_set each time
	FD_ZERO(server_set);
	FD_SET(client_socket, server_set);
	
	// select will get any messages from server without waiting
	int incoming_action = select(
		max_connection + 1, server_set, nullptr, nullptr, &quick
	);

	cout << to_string(incoming_action) << endl;

	if (FD_ISSET(client_socket, server_set)){
		incoming_stream = read(client_socket, buffer, STREAM_SIZE);

		if (incoming_stream == 0){
			// server_socket is no longer connected
			close(client_socket);
			FD_CLR(client_socket, server_set);
			cerr << "server disconnected, exiting." << endl;
			exit(1);
		}
		else {
			// handle the message
			// terminate char array for string handling
			buffer[incoming_stream] = '\0';
			message = buffer;

			cout << "Message received from server: " << message << endl;
			
		}
	}

	return message;
}





// note much of the network programming is similar to the server side code
// this also includes functionality for the GUI (using SDL)
int main(int argc, char *argv[]){
	int client_socket, client_id, address_length;
	struct hostent *server;
	char buffer[STREAM_SIZE];


	// get port and host from runtime args
	if (argc < 3) {
       printf("Usage: %s hostname port\n", argv[0]);
       exit(1);
    }
	int port = atoi(argv[2]);
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        cerr << "Undefined host, exiting...\n" << endl;;
        exit(1);
    }


	// or 0 as last param
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
    	cerr << "Error creating client socket, exiting..." << endl;
		exit(1);
	}

	
	// same address as server
	struct sockaddr_in address;
	
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;	// may need to get this from the server, i.e. server->h_addr
	address_length = sizeof(address);


	// attempt connection to server socket
	client_id = connect(client_socket, (struct sockaddr*) &address, address_length);
	if (client_id < 0){
        cerr << "Unable to connect to server; exiting..." << endl;
		exit(1);
	}


	// get connection message from server
	int out;
    out = read(client_socket, buffer, 255);
    if (out < 0){
    	cerr << "Error getting message from server, exiting..." << endl;
    	exit(1);
    }

    cout << "Message from server:\n\t" << buffer << endl;
    

    bool start = false;
    string message;

    // attempt to start the game 
    while (!start){
    	// clear buffer
    	memset(buffer, 0, 255);
    	fgets(buffer, 255, stdin);	// get client message from console

		// send message to server ("start" will confirm on server-side)
	    out = write(client_socket, buffer, strlen(buffer));
	    if (out < 0){
	        cerr << "Error sending message to server, exiting..." << endl;
	        exit(1);
	    }

	    // if started, open up the gui, then listen for your turn
	    if (strncmp(buffer, "start", 5) == 0){
	    	start = true;
	    }
    }




    // when game starts, load all media
    // and initialize all static SDL/GUI functionality

    // initialize sdl
    int success;
    success = SDL_Init(SDL_INIT_VIDEO);

    if (success < 0){
    	printf("Error on initialization: %s\nexiting...", SDL_GetError());
    	exit(1);
    }


    // initialize window for GUI
    SDL_Window* window = NULL;
    window = SDL_CreateWindow(
    	"CLUE!", 
    	SDL_WINDOWPOS_UNDEFINED, 
    	SDL_WINDOWPOS_UNDEFINED, 
    	SCREEN_WIDTH, 
    	SCREEN_HEIGHT, 
    	SDL_WINDOW_SHOWN
    );

    if (window == NULL){
		printf("Error on creating window: %s\nexiting...", SDL_GetError() );
		exit(1);
	}



	// initialize renderer (will draw on the window)
	SDL_Renderer* renderer = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (renderer == NULL){
		printf("couldn't make renderer; error: %s\n", SDL_GetError() );
	}
	else{
		// set renderer draw colour
		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );	// black

		// initialize png and jpg loading
		int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(img_flags) & img_flags)){
			printf("couldn't initialize png and jpg, error: %s\n", IMG_GetError());
		}
	}

	// load clue board image
	SDL_Texture* board = NULL;
    SDL_Rect backgroundRect = {0, 0, SCREEN_WIDTH,SCREEN_HEIGHT};

    board = load_image("images/board.png", renderer);
	if (board == NULL){
		cerr << "Couldn't load board... exiting\n" << endl;
		exit(1);
	}

	// LOAD ALL OTHER IMAGES HERE 





	// we need reading from socket to be instantaneous
	// since render is not maintainable
	// fd_set select is able to do that for us
	// define a quick timeout so select returns immediately
	fd_set server_set;	
	int max_connection = client_socket;
	struct timeval quick;
	quick.tv_sec = 0;
	quick.tv_usec = 100000;	// 0.1 seconds




	SDL_Event e;
	bool quit = false;
	bool premature_quit = false;	// if someone exits their window
	string action;
	// bool handled = false;

	// SDL_RenderClear(renderer);


	
	// this is the main loop for each client
	// different functionality depending on whether it is your turn
	// we also render every iteration, since the game state changes
	while (!quit){
		// wait on any message from server
		// different messages will correspond to different logic
		// e.g. turn start, or change in game state from other player's turn

		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, board, NULL, &backgroundRect);

		SDL_RenderPresent(renderer);	// update screen 

		cout << "plz" << endl;
		


		// quick ping server to see if any messages came in
		action = ping_server(
			&server_set, max_connection, client_socket, quick
		);

		cout << action << endl;

		if (action.compare(navigate_accuse_str) == 0){
			cout << "GO INTO MOUSE HANDLING HERE" << endl;
			// probably want an inner loop. we'll probably need to render in that inner loop too unfortunately
			/* e.g.
			while (move not handled){
				render
				while (SDL_PollEvent(&e)){
					etc
				}
			}
			*/
		}
		


		
		// get mouse information
		int mouse_output = handle_board_mouse();

		// SDL_WaitEvent(&e);	this is more efficient, but 
		// can freeze the board state if player is inactive
		// 
	    while (SDL_PollEvent(&e)){
	        if (e.type == SDL_QUIT){
	            quit = true;
	            premature_quit = true;
	        }
	        else if ((e.type == SDL_MOUSEBUTTONDOWN) && (e.button.button == SDL_BUTTON_LEFT)){
	        	if (mouse_output == -10){
	        		cout << "boundary click" << endl;
	        	}
	        	else if (mouse_output == 1){
	        		cout << "TOP LEFT CLICK!" << endl;
	        	}
	        	else if (mouse_output == 2){
	        		cout << "TOP RIGHT CLICK!" << endl;
	        	}
	        	else if (mouse_output == 3){
	        		cout << "BOTTOM LEFT CLICK!" << endl;
	        	}
	        	else if (mouse_output == 4){
	        		cout << "BOTTOM RIGHT CLICK!" << endl;
	        	}
	        	else {
	        		cout << "invalid click?" << endl;
	        	}
	            
	            // quit = true;
	        }	        
	    }

	    

	    
	}

	

	// at this point, game has ended.

    // clear memory used by SDL
	SDL_DestroyWindow( window );
    SDL_Quit();


    // will exit immediately; need to write code for gameplay continuation
    // while (true){

    // }

    close(client_socket);
    return 0;

}




