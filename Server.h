#pragma once

#include <iostream>

#include <stdio.h>  
#include <string.h>
#include <stdlib.h>  

#include <errno.h>  
#include <unistd.h>
#include <arpa/inet.h>   

#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/time.h>

#include <netinet/in.h>  

#define MAX_CLIENTS 6   // one per player


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

		int initialize();	// get client connections; return # clients
		int* get_socket_tracker();

		// void close_all();



	private:
		int socket_tracker[MAX_CLIENTS];
		int port, listening_socket;

		int stream_size;
		int max_pending, address_length;
		int max_clients, n_clients;

		struct sockaddr_in address;

		// fd_set is standard c++ code for handling multiple
		// clients without multithreading
		// multithreading is not scalable with # clients, 
		// and we have 6, which would not be nice to handle
		fd_set active_sockets;


};


