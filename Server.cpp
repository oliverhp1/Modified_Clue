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


	for (int i = 0; i < MAX_CLIENTS; i++){
		socket_tracker[i] = 0;
	}




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

}









