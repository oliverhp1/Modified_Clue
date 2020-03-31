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

	this->not_your_turn = "Invalid message: not your turn.\r\n";




}


/*
 * This method listens for and initializes all client connections
 * when all clients are connected (or one of them starts game),
 * this method is terminated
 */
void Server::initialize(){
	bool running = true;
	int temp_socket, new_client;

	// mostly helper variables for tracking socket related logic
	int connection_index, max_connection;
	int incoming_stream;
	char buffer[stream_size];

	for (int i = 0; i < max_clients; i++){
		socket_tracker[i] = 0; 
	}


	string connection_message = 
		"Connection confirmed. "
		"Type \"start\" when all players are connected.\r\n";
	string insufficient_message = "Only one client connected. At least "
		"two required.\r\n";
	string invalid_message = "Invalid message. Type \"start\" when ready\r\n";
	string start_message = "Game is starting.\r\n";
	

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
	max_connection = listening_socket;

	cout << "Listening on port: " << port << endl;

	fd_set active_sockets;




	

	while (running){
		// need to reset the fd_set since select() will clear it
		FD_ZERO(&active_sockets);
		FD_SET(listening_socket, &active_sockets);
		max_connection = listening_socket;

		for (int i = 0; i < max_clients; i++){
			temp_socket = socket_tracker[i];
			if (temp_socket > 0) FD_SET(temp_socket, &active_sockets);
			if (temp_socket > max_connection) max_connection = temp_socket;
		}
		

		// listen for an action on any socket
		int incoming_action = select(
			max_connection + 1, &active_sockets, nullptr, nullptr, nullptr
		);


		if (FD_ISSET(listening_socket, &active_sockets)){
			// client attempting to connect
			// accept the connection and add it to fd_set (and socket tracker)
			cout << "Connection attempt received...\n" << endl;
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

			// TODO: PERHAPS USE THESE INDICES TO ASSIGN CHARACTERS LATER ON

			while (socket_tracker[connection_index] != 0){connection_index++;}
			socket_tracker[connection_index] = new_client;

			cout << "Successful new connection " << new_client 
				 << " at index " << connection_index << endl;
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

	cout << "Game starting. Notifying all " << n_clients 
		 << " clients\r\n" << endl;

	for (int i = 0; i < n_clients; i++){
		// check for messages from all clients
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
	// reset fd_set
	int temp_socket;
	bool received = false;
	int max_connection = 0;
	int incoming_stream;
	char buffer[STREAM_SIZE];

	// if error, use this->
	for (int i = 0; i < max_clients; i++){
		temp_socket = socket_tracker[i];
		if (temp_socket > 0) FD_SET(temp_socket, &active_sockets);
		if (temp_socket > max_connection) max_connection = temp_socket;
	}

	while (!received){
		// listen for an action on any socket
		int incoming_action = select(
			max_connection + 1, &active_sockets, nullptr, nullptr, nullptr
		);

		// when received, check where it came from
		for (int i = 0; i < max_clients; i++){
			// check for messages from all clients
			temp_socket = socket_tracker[i];


			if (FD_ISSET(temp_socket, &active_sockets)){
				if (temp_socket != socket_id){
					// someone else is trying to send something
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
						// buffer[incoming_stream] = '\0';	 // is this necessary?
						received = true;	// don't think this is needed either
						string output(buffer);
						return output;
					}
				}
			}
		}
	}
	cout << "debug: loop broken in Server.receive_communication\r\n" << endl;
}




int* Server::get_socket_tracker(){
	return socket_tracker;
}

int Server::get_n_clients(){
	return n_clients;
}

/* 
void close_all(){		go through each of socket_tracker[i] and close each
	cout << "Closing all connections... \r\n" << endl;

	for (int i = 0; i < n_clients; i++){
		close(socket_tracker[i]);
		FD_CLR(socket_tracker[i], &active_sockets);
	}	

	cout << "Confirmed; exiting program.\r\n" << endl;
}
*/

