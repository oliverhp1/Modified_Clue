#include "Server.h"

using namespace std;
/*
 * Server constructor
 * parameters: 
 	- port (which port clients will connect to)
 	- max_clients (max total client connections)
 	- max_pending (max pending connections on listening socket)
 	- stream_size (size of stream to get from connections)
 */
Server::Server(int port, int max_clients, int max_pending, int stream_size){
	// initialize relevant server variables
	this->port = port;
	this->max_clients = max_clients;
	this->max_pending = max_pending;
	this->stream_size = stream_size;
	this->n_clients = 0;


	for (int i = 0; i < max_clients; i++){
		socket_tracker[i] = 0;
	}

}


/*
 * This method listens for and initializes all client connections
 * when all clients are connected (or one of them starts game),
 * this method is terminated
 */
void Server::initialize(){
	// initialize socket that will listen for connections
	listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listening_socket == 0){
		cerr << "Failed to create socket; exiting program..." << endl;
		exit(1);
	}

	
	struct sockaddr_in address;
	
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	address_length = sizeof(address);


	// bind ip address and port to socket
	bind(listening_socket, (sockaddr*)&address, sizeof(address));

	// set socket as listening socket
	listen(listening_socket, max_pending);

	cout << "Listening on port: " << port << endl;

	fd_set active_sockets = connect_all();

	cout << "Game starting. Notifying all " << n_clients 
		 << " clients\r\n" << endl;

	int temp_socket;

	for (int i = 0; i < n_clients; i++){
		// send start message to all clients
		temp_socket = socket_tracker[i];

		if (temp_socket > 0){
			send(temp_socket,
				start_message.c_str(),
				start_message.size(),
				0
			);
		}
	}

	// 
	this->active_sockets = active_sockets;

	return ;
}




// this will receive a char buffer from socket_id
// if we receive messages from other sockets, send back warning message
string Server::receive_communication(int socket_id){
	int temp_socket;
	bool received = false;
	int max_connection, incoming_stream;
	char buffer[STREAM_SIZE];

	while (!received){
		// if error, make copy of active_sockets first
		// reset each time we loop over clients, otherwise infinite loop
		// is possible
		max_connection = reset(&active_sockets);
		if (listening_socket > max_connection){
			max_connection = listening_socket;
		}

		// listen for an action on any socket
		int incoming_action = select(
			max_connection + 1, &active_sockets, nullptr, nullptr, nullptr
		);

		// when received, check where it came from
		for (int i = 0; i < n_clients; i++){
			// check for messages from active clients
			temp_socket = socket_tracker[i];

			if (FD_ISSET(temp_socket, &active_sockets)){
				if (temp_socket != socket_id){
					// someone else is trying to send something
					// need to read it anyways to clear the stream
					incoming_stream = read(temp_socket, buffer, stream_size);

					// then notify client
					send(temp_socket, 
						not_your_turn.c_str(), 
						not_your_turn.size(), 
						0
					);
				}
				else {
					// got desired message
					incoming_stream = read(temp_socket, buffer, stream_size);

					if (incoming_stream == 0){
						// temp_socket is no longer connected
						getpeername(
							temp_socket, 
							(struct sockaddr*)&address,  
							(socklen_t*)&address_length
						);
						cout << "Client disconnected: " 
							 << ntohs(address.sin_port);

						close(temp_socket);
						socket_tracker[i] = 0;
						FD_CLR(temp_socket, &active_sockets);
					}
					else {
						// handle the message
						// terminate char array for string handling
						// the last 2 characters will be newline, ?
						// so terminate before that
						// cout << "incoming stream: " << incoming_stream << endl;
						buffer[incoming_stream - 2] = '\0';	 
						received = true;	// not really necessary

						string output(buffer);
						return output;
					}
				}
			}
		}
	}

	cerr << "DEBUG: loop broken in Server.receive_communication" << endl;
	exit(1);
}

fd_set Server::connect_all(){
	// helper variables for tracking fd_set logic
	int new_client, max_connection, connection_index;
	int temp_socket, incoming_stream;
	char buffer[stream_size];

	// several starting conditions: max clients or manual start
	bool running = true;	

	while (running){
		// need to reset the fd_set each iteration since select will destroy it
		max_connection = reset(&active_sockets);

		if (listening_socket > max_connection){
			max_connection = listening_socket;
		}

		// listen for an action on any socket
		int incoming_action = select(
			max_connection + 1, &active_sockets, nullptr, nullptr, nullptr
		);


		if (FD_ISSET(listening_socket, &active_sockets)){
			// client attempting to connect
			// accept the connection and add it to fd_set (and socket tracker)
			cout << "Connection attempt received..." << endl;
			new_client = accept(
				listening_socket, 
				(struct sockaddr *)&address, 
				(socklen_t*)&address_length
			);

			if (new_client < 0){
				cerr << "Failed to connect; exiting..." << endl;
				exit(1);
			}

			// add new connection
			FD_SET(new_client, &active_sockets);
			n_clients++;

			connection_index = 0;

			// track it in our array
			// these indices are used to assign players 
			while (socket_tracker[connection_index] != 0){connection_index++;}
			socket_tracker[connection_index] = new_client;

			cout << "Successful new connection " << new_client 
				 << " at index " << connection_index << "\n\n";
			if (new_client > max_connection){
				max_connection = new_client;
			}

			// give client instructions for starting game
			send(
				new_client, 
				connection_message.c_str(), 
				connection_message.size(), 
				0
			);


		}
		else {
			// it's an inbound message from a client
			for (int i = 0; i < max_clients; i++){
				// check for messages from all clients
				temp_socket = socket_tracker[i];

				if (FD_ISSET(temp_socket, &active_sockets)){
					incoming_stream = read(temp_socket, buffer, stream_size);

					if (incoming_stream == 0){
						// temp_socket is no longer connected
						getpeername(
							temp_socket, 
							(struct sockaddr*)&address,  
							(socklen_t*)&address_length
						);
						cout << "Client disconnected: " 
							 << ntohs(address.sin_port);

						close(temp_socket);
						socket_tracker[i] = 0;
						FD_CLR(temp_socket, &active_sockets);
					}
					else {
						// handle the message
						// terminate char array for string handling
						buffer[incoming_stream] = '\0';

						printf(
							"Message received from socket %d: %s", 
						    temp_socket,
						    buffer
						);

						if (strncmp(buffer, "start", 5) == 0){
							// someone started the game
							if (n_clients == 1){
								send(temp_socket,
									insufficient_message.c_str(),
									insufficient_message.size(),
									0
								);
							}
							else {
								running = false;
							}
							
						}
						else {
							send(temp_socket, 
								invalid_message.c_str(), 
								invalid_message.size(), 
								0
							);
						}
						
					}
				}
			}
		}

		// don't allow more than 6 clients
		if (n_clients == max_clients){
			running = false;
		}
		else if (n_clients > max_clients){
			// should never get here; leave for debugging purposes
			cerr << "Max clients exceeded; exiting..." << endl;
			exit(1);
		}
	}
	return active_sockets;
}



string Server::connection_message = 
	"Connection confirmed. "
	"Type \"start\" when all players are connected.\r\n";

string Server::insufficient_message = 
	"Only one client connected. At least "
	"two required.\r\n";

string Server::invalid_message = "Invalid message. Type \"start\" when ready\r\n";

string Server::start_message = "Game is starting.\r\n";

string Server::not_your_turn = "Invalid message: not your turn.\r\n";


int Server::reset(fd_set *copy){
	int max_connection = 0;
	int temp_socket;

	FD_ZERO(copy);
	FD_SET(listening_socket, copy);
	
	// if we need to remove players dynamically, iterate over max_clients
	// and check if temp_socket is positive
	for (int i = 0; i < n_clients; i++){
		temp_socket = socket_tracker[i];
		FD_SET(temp_socket, copy);
		if (temp_socket > max_connection) max_connection = temp_socket;
	}

	return max_connection;
}


int* Server::get_socket_tracker(){
	return socket_tracker;
}

int Server::get_n_clients(){
	return n_clients;
}


void Server::close_all(){		
	cout << "Closing all connections... \r\n" << endl;

	for (int i = 0; i < n_clients; i++){
		close(socket_tracker[i]);
		FD_CLR(socket_tracker[i], &active_sockets);
	}	

	cout << "Confirmed; exiting program.\r\n" << endl;
}



