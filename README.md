# Clue

At this point (initial commit), to run, open a terminal console, cd to the directory, run `make clean`, then run `make` and finally `./game`  

This will set up the server, which will listen for client connections  

To connect from the client side, either:  

    1. open a new terminal, then `telnet localhost PORT`  

    2. using a different machine, `telnet <server-ip-address> PORT`  

where PORT is defined in ClueLess.cpp, and server-ip-address is the ip address of the machine that server is being run on.

example: `telnet localhost 8000`
