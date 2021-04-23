To compile the server:

g++ -o final_server final_server.cpp -lws2_32 -lwinmm

To compile the client:

g++ -Wall -D__LINUX_ALSA__ -o final final.cpp RtMidi.cpp -lasound -lpthread

Then run ./final_server on the server and ./final on the client.

The programs may not work on your machine, I set the IP address custom in the code.

Requires LoopMIDI

[Link to demo](https://www.youtube.com/watch?v=TV7AIoCRtCs)