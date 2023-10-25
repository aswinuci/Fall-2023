#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_socket, client_socket;
    char message_from_server[100] = "It's going great!";
    char client_message_to_server[100];
    
    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    
    // Define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(4123);
    server_address.sin_addr.s_addr = INADDR_ANY;
    
    // Bind the socket
    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    
    // Listen for incoming connections
    listen(server_socket, 1000);
    
    // Accept a connection
    client_socket = accept(server_socket, NULL, NULL);
    
    // Receive a message from the client (Mac)
    recv(client_socket, client_message_to_server, sizeof(client_message_to_server), 0);
    printf("Received from Mac: %s\n", client_message_to_server);
    
    // Send a message to the client (Mac)
    send(client_socket, message_from_server, sizeof(message_from_server), 0);
    
    // Close the sockets
    close(client_socket);
    close(server_socket);
    
    return 0;
}
