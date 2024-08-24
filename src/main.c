#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_MODE 'S'
#define CLIENT_MODE 'C'

// Define constants
#define BUFFER_SIZE 1024
#define PORT 9002

// Function prototypes
int run_server();
int run_client();

int main(){
    char mode;

    printf("Run the example in (s)erver or (c)lient mode?\n");
    scanf("%c", &mode);
    char mode_capitalized = toupper(mode);

    if(mode_capitalized == SERVER_MODE)
    {
        printf("Application has started at server mode. Waiting for connections...\n");
        run_server();
    }
    else if(mode_capitalized == CLIENT_MODE)
    {
        printf("Application has started at client mode.\n");
        run_client();
    }
    else
    {
        printf("The value you entered is invalid: %c\n", mode_capitalized);
        return -1;
    }

    return 0;
    
}

int run_client()
{
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify the address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_result = connect(network_socket, 
                                    (struct sockaddr *)&server_address, 
                                    sizeof(server_address));

    if(connection_result < 0)
    {
        printf("Connection could not be established.\n");
        return connection_result;
    }

    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);

    // print out the server's response
    printf("%s", server_response);

    close(network_socket);

    return 0;
}

int run_server()
{
    char server_message[256] = "You have reached the server!\n";

    // create the server socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0); 

    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    //bind the socket to our specified IP and port
    bind(server_socket, (struct  sockaddr*) &server_address, sizeof(server_address));
    listen(server_socket, 5);

    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);

    send(client_socket, server_message, sizeof(server_message), 0);
    
    close(server_socket);
    return 0;
}