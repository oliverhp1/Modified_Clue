#include "Client.h"
#include <netdb.h>


using namespace std;



// note much of this is similar to the server side code
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
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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


    printf("Please enter the message: ");
    fgets(buffer,255,stdin);
    int action;

    // send message to server
    action = write(client_socket, buffer, strlen(buffer));
    if (action < 0){
        cerr << "Error sending message to server, exiting..." << endl;
        exit(1);
    }

    // get message from server
    action = read(client_socket, buffer, 255);
    if (action < 0){
    	cerr << "Error getting message from server, exiting..." << endl;
    	exit(1);
    }

    printf("Message from server!\n\t%s\n", buffer);
    string message(buffer);

    if (message.compare("Connection confirmed. Type \"start\" when all players are connected.")){
    	cout << "MESSAGE FROM SERVER SUCCESSFULLY HANDLED" << endl;
    }




    // will exit immediately; need to write code for gameplay continuation
    // while (true){

    // }

    close(client_socket);
    return 0;

}








