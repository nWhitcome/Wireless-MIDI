#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <sstream>
#include <sys/types.h>
#include <sys/inotify.h>
#include <iostream>
#include <cstdlib>
#include "RtMidi.h"
#define PORT 8888 

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))

void mycallback( double deltatime, std::vector< unsigned char > *message, void *userData)
{
  std::stringstream ss;
  unsigned int nBytes = message->size();
  for ( unsigned int i=0; i<nBytes; i++ ){
    ss << std::hex << (int)message->at(i);
    if((int)message->at(i) == 0)
	  ss << '0';
  }
  if ( nBytes > 0 ){
    send((int)userData , ss.str().c_str() , strlen(ss.str().c_str()) , 0 );
    //std::cout << ss.str() <<std::endl;
  }
}

int main()
{
  int sock = 0; 
  struct sockaddr_in serv_addr; 
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  { 
      printf("\n Socket creation error \n"); 
      return -1; 
  } 
   
  serv_addr.sin_family = AF_INET; 
  serv_addr.sin_family = AF_INET; 
  serv_addr.sin_port = htons(PORT); 
       
  // Convert IPv4 and IPv6 addresses from text to binary form 
  if(inet_pton(AF_INET, "192.168.0.87", &serv_addr.sin_addr)<=0)  
  { 
      printf("\nInvalid address/ Address not supported \n"); 
      return -1; 
  } 
   
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
  { 
      printf("\nConnection Failed \n"); 
      return -1; 
  }
  RtMidiIn *midiin = new RtMidiIn();
  // Check available ports.
  unsigned int nPorts = midiin->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No ports available!\n";
    goto cleanup;
  }
  midiin->openPort( 1 );
  // Set our callback function.  This should be done immediately after
  // opening the port to avoid having incoming messages written to the
  // queue.
  midiin->setCallback(&mycallback, (void *) sock);
  // Don't ignore sysex, timing, or active sensing messages.
  midiin->ignoreTypes( false, false, false );
  std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
  char input;
  std::cin.get(input);
  // Clean up
 cleanup:
  delete midiin;
  return 0;
}
