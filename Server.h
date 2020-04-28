#pragma once

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

#include "globals.h"


#define MAX_CLIENTS 6   // one per player
#define STREAM_SIZE 1024



using namespace std;


/*
 * this server class serves to connect clients.
 * when ready, any client can kick off the game,
 * provided there is more than one client connected.

 * to handle multiple client connections at once, we use fd_set.
 * the brute force approach would be making a separate thread per client,
 * but as we have up to 6 clients, it would quickly become untenable to track
 * each one individually.
 */
class Server{
	public:
		// port, max clients, max pending connections, stream size
		Server(int, int, int, int);
		// ~Server();

		void initialize();	// get client connections; return # clients
		int* get_socket_tracker();

		string receive_communication(int);	// socket id

		int get_n_clients();

		void close_all();



	private:
		int socket_tracker[MAX_CLIENTS];
		int port, listening_socket;

		int stream_size;
		int max_pending, address_length;
		int max_clients, n_clients;

		// common communication messages to send to clients
		static string connection_message, start_message;
		static string insufficient_message, invalid_message, not_your_turn;


		struct sockaddr_in address;

		// fd_set is standard c++ code for handling multiple
		// clients without multithreading
		// multithreading is not scalable with # clients, 
		// and we have 6, which would not be nice to handle
		fd_set active_sockets;


		int reset(fd_set*);
		fd_set connect_all();



};


