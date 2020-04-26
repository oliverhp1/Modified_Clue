#include "Server.h"

using namespace std;


/*
 * this client class serves to connect to the server.
 * after connected, we need to make custom functionality
 * for handling all inputs and outputs.
 * in particular, all messages from server will represent changes
 * in game state, and all messages to server will represent
 * the action the client wants to take.
 */
// class Client {
// 	public:
// 		// port, max clients, max pending connections, stream size
// 		Client(int, int, int, int);
// 		// ~Client();

// 		void initialize();	// connect to server
		// void send_communication();	// send to server


// 		string receive_communication(int);	// socket id




// 	private:

// 		// common communication messages to send to clients
// 		static string connection_message, start_message;
// 		static string insufficient_message, invalid_message, not_your_turn;



// };


