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

void *fileServer(void *);
void *fileClient(void *);

int main(){
	int c;
	char* sourceFile = "example.txt";
	char* outputFile = "recived_file.txt";
	pthread_t serverThread, clientThread;

	pthread_create(&serverThread, NULL, fileServer, (void*)sourceFile);

	pthread_join(serverThread, NULL);

	return 0;
}

void *fileServer(void *fileName){
	
    //intialize varibles
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr;
    FILE *filePointer;
    ssize_t bytesRead;
    char buffer[BUFFER_SIZE];

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        socklen_t clientAddrLen;
        clientAddrLen = sizeof(struct sockaddr_in);

        clientSocket = accept(serverSocket, (struct sockaddr *)&serverAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Connection accept failed");
            exit(EXIT_FAILURE);
        }

	//test
	recv(clientSocket,buffer,BUFFER_SIZE,0);

        printf("Client connected and the message: %s\n",buffer);

        filePointer = fopen((char *)fileName, "rb");
        if (filePointer == NULL) {
            perror("File opening failed");
            exit(EXIT_FAILURE);
        }

        while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, filePointer)) > 0) {
            send(clientSocket, buffer, bytesRead, 0);
        }

        fclose(filePointer);
        close(clientSocket);
	iter++;
    }

    close(serverSocket);

    return NULL;
}

void* fileClient(void *fileName) {
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

    FILE *filePointer = fopen((char*)fileName, "wb");
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
