#include "Server.h"
// #include "client.h"


#define PORT 10000
#define MAX_PENDING_CONN 3
#define STREAM_SIZE 1024

using namespace std;



int main(int argc, char *argv[]){
	Server server(PORT, MAX_CLIENTS, MAX_PENDING_CONN, STREAM_SIZE);

	server.initialize();

	return 0;
}