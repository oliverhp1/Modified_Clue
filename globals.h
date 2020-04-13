#pragma once


#include <iostream>
// #include <boost/unordered_map.hpp>
// #include <boost/config.hpp>
#include <unordered_map>

#include <stdio.h>  
#include <string.h>
#include <stdlib.h>  
#include <vector> 

#include <errno.h>  
#include <unistd.h>
#include <arpa/inet.h>   

#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/time.h>

#include <netinet/in.h>  

using namespace std;


// use these later for the gui
// extern const int SCREEN_WIDTH;
// extern const int SCREEN_HEIGHT;

// fd_set doesn't work with c++ 11, 
// so we might need to use this instead of std::unordered_map
// extern boost::unordered_map<int, string> locations;
// extern boost::unordered_map<int, string> cards;

