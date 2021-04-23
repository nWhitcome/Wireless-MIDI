#include <io.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define DEFAULT_BUFLEN 512

void RemoveChars(char *s, char c);
char* concat(const char *s1, const char *s2);
char* swapChars(char * input);

int main(int argc , char *argv[])
{
	WSADATA wsa;
	SOCKET s , new_socket;
	struct sockaddr_in server , client;
	int c;
	char *finalMessage;
	char *finalMessage2;
	int iResult;
	char recvbuf[DEFAULT_BUFLEN + 1];
    int recvbuflen = DEFAULT_BUFLEN + 1;
	unsigned long result;
	HMIDIOUT      outHandle;
	MIDIOUTCAPS     moc;

	//Standard Windows socket startup
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	
	printf("Initialised.\n");
	
	//Create a socket
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}

	printf("Socket created.\n");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	
	//Bind
	if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	
	puts("Bind done");

	//Listen to incoming connections
	listen(s , 3);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	
	c = sizeof(struct sockaddr_in);

	//Check for client connections
	while( (new_socket = accept(s , (struct sockaddr *)&client, &c)) != INVALID_SOCKET )
	{
		puts("Connection accepted");

		//Get the third MIDI output, which happens to be the loopMIDI output on my computer. May need modification on another system.
		if (!midiOutGetDevCaps(3, &moc, sizeof(MIDIOUTCAPS)))
		{
			/* Display its Device ID and name */
			printf("%s\r\n", moc.szPname);
			result = midiOutOpen(&outHandle, 3, 0, 0, CALLBACK_WINDOW);
			if (result)
			{
				printf("There was an error opening MIDI Mapper!\r\n");
			}
			do {
				//Receive the data from the socket and put it into a buffer
				iResult = recv(new_socket, recvbuf, recvbuflen, 0);
				if ( iResult > 0 ){
					recvbuf[iResult] = '\0';
					
					//Modify the input so that it is in a proper format to be sent to the MIDI output
					RemoveChars(recvbuf, ' ');
					finalMessage = concat("0x00", swapChars(recvbuf));
					int number = (int)strtol(finalMessage, NULL, 0);
					if(!result){
						midiOutShortMsg(outHandle, number);
					}
				}
				else if ( iResult == 0 )
					printf("Connection closed\n");
				else
					printf("recv failed: %d\n", WSAGetLastError());

			} while( iResult > 0 );
			midiOutClose(outHandle);
		}
	}
	
	//Exit if the socket is invalid
	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d" , WSAGetLastError());
		return 1;
	}

	closesocket(s);
	WSACleanup();
	return 0;
}

//Re-order the MIDI message to the proper format for the Windows MIDI output
char* swapChars(char * input){
	char *returned = (char *)malloc(strlen(input) + 1);
	size_t len = strlen(input);
	memcpy(returned, &input[4], 2);
	memcpy(&returned[2], &input[2], 2);
	memcpy(&returned[4], &input[0], 2);
	return returned;
}

//Remove specific characters from a string
void RemoveChars(char *s, char c)
{
    int writer = 0, reader = 0;
    while (s[reader])
    {
        if (s[reader]!=c) 
        {   
            s[writer++] = s[reader];
        }
        reader++;       
    }
    s[writer]=0;
}

//Custom concat function for strings
char* concat(const char *s1, const char *s2)
{
    char *result = (char *)malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}