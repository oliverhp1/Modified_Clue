#include "Server.h"


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

using namespace std;





/*
 * this client class serves to connect to the server
 * after connected, we also use this class to send/receive
 * messages from the server.
 * this includes rendering the board and handling all events.
 * in particular, all messages from server will represent changes
 * in game state, and all messages to server will represent
 * the action the client wants to take.
 */



// class Client {
// 	public:
// 		// port, max clients, max pending connections, stream size
// 		// Client(int, int, int, int);
// 		// ~Client();

// 		void initialize();	// connect to server
// 		void send_communication();	// send to server


// 		string receive_communication(int);	// socket id




// 	private:

// 		// common communication messages to send to clients
// 		static string connection_message, start_message;
// 		static string insufficient_message, invalid_message, not_your_turn;



// };


