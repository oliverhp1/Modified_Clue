# Clue

This is a rendition of the popular detective board game, Clue.  The main difference is navigation: rather than rolling a dice and moving a specific number of squares, we just have one space per hallway and room, and you can move one space per turn.

Before running, you will need to install the libraries: SDL2, SDL2_image, SDL2_ttf, and boost.  The easiest way to do this on Mac OS would be to use homebrew (brew install SDL2, etc).  For linux/unix machines apt-get should work (but has not been tested).

To run, open a terminal console, cd to the directory, run `make clean`, then run `make` and finally `./server [PORT] [number of players]`.  Make sure to select a port that is not in use by other software on your computer. 10001-10009 tend to work fine. 
 
This will set up the server, which will listen for client connections. 

To connect from the client side, either:  

    1. on the same computer, open a new terminal, then `./client localhost [PORT]`  

    2. using a different computer, `./client <server-ip-address> PORT`  

where PORT is whatever the server was run using, and server-ip-address is the ip address of the machine that server is being run on.


Next steps for development:
render a detective notepad with checkbox functionality
do not use ping_server if we require a response quickly from the server
animate motion
add music
allow "play again" option after game ends
