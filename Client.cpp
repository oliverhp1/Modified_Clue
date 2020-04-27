#include "Client.h"
#include <netdb.h>

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

using namespace std;






SDL_Texture* loadFromFile(string path, SDL_Renderer* renderer){
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






// note much of this is similar to the server side code (aside from the sdl functionality)
int main(int argc, char *argv[]){
	// int client_socket, client_id, address_length;
	// struct hostent *server;
	// char buffer[STREAM_SIZE];

	// // get port and host from runtime args
	// if (argc < 3) {
 //       printf("Usage: %s hostname port\n", argv[0]);
 //       exit(1);
 //    }
	// int port = atoi(argv[2]);
 //    server = gethostbyname(argv[1]);

 //    if (server == NULL) {
 //        cerr << "Undefined host, exiting...\n" << endl;;
 //        exit(1);
 //    }

	// // or 0 as last param
	// client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// if (client_socket < 0) {
 //    	cerr << "Error creating client socket, exiting..." << endl;
	// 	exit(1);
	// }

	
	// // same address as server
	// struct sockaddr_in address;
	
	// address.sin_family = AF_INET;
	// address.sin_port = htons(port);
	// address.sin_addr.s_addr = INADDR_ANY;	// may need to get this from the server, i.e. server->h_addr
	// address_length = sizeof(address);


	// // attempt connection to server socket
	// client_id = connect(client_socket, (struct sockaddr*) &address, address_length);
	// if (client_id < 0){
 //        cerr << "Unable to connect to server; exiting..." << endl;
	// 	exit(1);
	// }


 //    printf("Please enter the message: ");
 //    fgets(buffer,255,stdin);
 //    int action;

 //    // send message to server
 //    action = write(client_socket, buffer, strlen(buffer));
 //    if (action < 0){
 //        cerr << "Error sending message to server, exiting..." << endl;
 //        exit(1);
 //    }

 //    // get message from server
 //    action = read(client_socket, buffer, 255);
 //    if (action < 0){
 //    	cerr << "Error getting message from server, exiting..." << endl;
 //    	exit(1);
 //    }

 //    printf("Message from server!\n\t%s\n", buffer);
 //    string message(buffer);


    // FIRST ATTEMPT AT RENDERING!!

    int success;
    SDL_Window* window = NULL;
    // SDL_Surface* screen_surface = NULL;

    success = SDL_Init(SDL_INIT_VIDEO);

    if (success < 0){
    	printf("Error on initialization: %s\nexiting...", SDL_GetError());
    	exit(1);
    }

    window = SDL_CreateWindow("CLUE!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL){
		printf("Error on creating window: %s\nexiting...", SDL_GetError() );
		exit(1);
	}

	// screen_surface = SDL_GetWindowSurface(window);

	// SDL_FillRect( screen_surface, NULL, SDL_MapRGB( screen_surface->format, 0xFF, 0xFF, 0xFF ) );
            
	//Update the surface
	// SDL_UpdateWindowSurface( window );




	// load image and render
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL){
		printf("couldn't make renderer error: %s\n", SDL_GetError() );
		// success = false;
	}
	else{
		//set renderer draw colour
		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );	// black

		//initialize png loading
		int imgFlags = IMG_INIT_PNG|IMG_INIT_JPG;
		if (!(IMG_Init(imgFlags) & imgFlags)){
			printf("couldn't initialize png and jpg, error: %s\n", IMG_GetError() );
			// success = false;
		}
	}

	SDL_Texture* board = NULL;
    SDL_Rect backgroundRect = {0, 0, SCREEN_WIDTH,SCREEN_HEIGHT};

    board = loadFromFile("images/board.png", renderer);
	if (board == NULL){
		cerr << "Couldn't load board... exiting\n" << endl;
		exit(1);
	}
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, board, NULL, &backgroundRect);
	SDL_RenderPresent(renderer);	// update screen






	// hold image until quit

	SDL_Event e;
	bool quit = false;
	while (!quit){
	    while (SDL_PollEvent(&e)){
	        if (e.type == SDL_QUIT){
	            quit = true;
	        }
	        if (e.type == SDL_KEYDOWN){
	            quit = true;
	        }
	        if (e.type == SDL_MOUSEBUTTONDOWN){
	            quit = true;
	        }
	    }
	}

	

    
    

	

 //    // if (message.compare("Connection confirmed. Type \"start\" when all players are connected.")){
 //    	cout << "MESSAGE FROM SERVER SUCCESSFULLY HANDLED" << endl;

 //    	while (true){
 //    	}
		

    // }

    // clear memory used by SDL
	SDL_DestroyWindow( window );
    SDL_Quit();


    // will exit immediately; need to write code for gameplay continuation
    // while (true){

    // }

    // close(client_socket);
    return 0;

}
