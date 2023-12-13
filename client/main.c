#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define PORT 9002 
#define BUFFER_SIZE 1024

int main () {
    int clientSocket;
    struct sockaddr_in serverAddr;
    ssize_t bytesReceived;
    char buffer[BUFFER_SIZE];

    // Creating socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Change to server IP if not localhost
    serverAddr.sin_port = htons(PORT);

    // Connecting to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    FILE *filePointer = fopen("recvied_file.txt", "wb");
    if (filePointer == NULL) {
        perror("File opening failed");
        exit(EXIT_FAILURE);
    }

    //test
    printf("Enter message: ");
    scanf("%[^\n]",buffer);
    send(clientSocket, buffer,BUFFER_SIZE,0);

    // Receiving file contents from server
    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytesReceived, filePointer);
    }

    fclose(filePointer);
    close(clientSocket);

    printf("File received successfully\n");

    return 0;
}
