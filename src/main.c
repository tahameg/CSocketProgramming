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

#define RED_BOLD "\033[1;31m"
#define BLUE_BOLD "\033[1;34m"
#define RESET_STYLE "\033[0m"


// Define constants
#define BUFFER_SIZE 1024
#define PORT 9002

// Function prototypes
int run_server();
int run_client();

/// This function gets the prompt until reaches to a null character and 
/// returns number of characters written including the null character. 
/// Function guarantees to end with null character.
int get_prompt(char* write_buffer, int max_buffer_size)
{
    char c;
    int counter = 0;
    while((c = getchar()) != '\n' && counter+1 < max_buffer_size)
    {
        write_buffer[counter++] = c;
    }

    write_buffer[counter++] = '\0';
    return counter;
}


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

    printf("Connecting to the socket...\n");
    int connection_result = connect(network_socket, 
                                    (struct sockaddr *)&server_address, 
                                    sizeof(server_address));

    if(connection_result < 0)
    {
        printf("Connection could not be established.\n");
        return connection_result;
    }

    printf("Expecting messages...\n");
    char server_response[256];
    char client_message[256];
    
    //remove the new line character.
    get_prompt(client_message, sizeof(client_message));
    
    while(1)
    {
        recv(network_socket, server_response, sizeof(server_response), 0);
        printf(RED_BOLD"Server:"RESET_STYLE" %s\n", server_response);
        
        if(strcmp(server_response, "exit") == 0){
            printf("Server concluded the transaction.\n");
            break;
        }

        get_prompt(client_message, sizeof(client_message));
        printf(BLUE_BOLD"You:"RESET_STYLE" %s\n", client_message);

        send(network_socket, client_message, sizeof(client_message), 0);
    }

    close(network_socket);

    return 0;
}

int run_server()
{
    char server_message[256] = "You have reached the server!\n";
    char client_response[256];

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
    printf("Started accepting connections...\n");


    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);
    
    send(client_socket, server_message, sizeof(server_message), 0);
    
    //remove the new line character.
    get_prompt(server_message, sizeof(server_message));

    while(1)
    {
        //Wait for response from the client.
        recv(client_socket, &client_response, sizeof(client_response), 0);
        
        //Print client's response.
        printf(RED_BOLD"Client %d:"RESET_STYLE" %s\n", client_socket, client_response);

        //Get the user's message.
        get_prompt(server_message, sizeof(server_message));
        
        //Send the message
        send(client_socket, server_message, sizeof(server_message), 0);
        
        //Print the user's message
        printf(BLUE_BOLD"You:"RESET_STYLE"%s\n", server_message);

        if(strcmp(server_message, "exit") == 0)
        {
            break;
        }
    }

    close(server_socket);
    return 0;
}