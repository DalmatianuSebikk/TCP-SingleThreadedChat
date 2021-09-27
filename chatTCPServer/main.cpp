#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#define DEFAULT_PORT "8080"
#define MAXIMUM_BUFF 256
#pragma comment(lib, "Ws2_32.lib")

// initialzie() -> socket() -> bind() -> listen() -> accept() -> send / recv()

void exitProgram(){
    WSACleanup();
    exit(EXIT_FAILURE);
}

void handle(SOCKET clientSocket) {
    if(clientSocket == INVALID_SOCKET) {
        std::cout << "Socket is invalid " << WSAGetLastError();
        exitProgram();
    }
    else{
        std::cout << "Connection has been made! Welcome to the chat. \n";
    }
}

void inOut(SOCKET clientSocket) {
    char buffer[MAXIMUM_BUFF];
    char message[150];

    int iResult, sendResult;
    char welcomeMsg[] = "Welcome!";
    sendResult = send(clientSocket, welcomeMsg, sizeof(welcomeMsg), 0);
    do{
        iResult = recv(clientSocket, buffer, MAXIMUM_BUFF, 0);
        if(iResult > 0) {
            std::cout << "Message received: ";
            for(int i = 0; i< strlen(buffer); i++){
                std::cout << buffer[i];
            }
            std::cout << "\n";

            std::cin.getline(message, 150);
            sendResult = send(clientSocket, message, sizeof(message), 0);
            if(sendResult == SOCKET_ERROR) {
                std::cout << "send failed";
                closesocket(clientSocket);
                exitProgram();
            }

        }
        else if(iResult == 0){
            std::cout << "Connection closed.\n";
        }
        else{
            std::cout << "Receiving failed";
            closesocket(clientSocket);
            exitProgram();
        }

    } while(iResult > 0);
}

int main() {
//    initializing the winsock app.
    WSAData wsaData;
    SOCKET listenSocket = INVALID_SOCKET;

    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0) {
        std::cout << "Startup failed: " << iResult << "\n";
        exitProgram();
    }
    else{
        std::cout << "WSA Started..\n";
    }

//    creating the server socket
    struct addrinfo *result = NULL , *ptr = NULL, hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET; // ipv4
    hints.ai_socktype = SOCK_STREAM; // specify the stream socket.
    hints.ai_protocol = IPPROTO_TCP; // TCP protocol.
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult != 0) {
        std::cout << "getAddrInfo failed: " << iResult << '\n';
        exitProgram();
    }
    std::cout << "AddrInfo successfully.. \n";

//    socket()
    listenSocket = socket(result -> ai_family, result -> ai_socktype, result -> ai_protocol);
    if(listenSocket == INVALID_SOCKET) {
        std::cout << "Invalid socket error: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        exitProgram();
    }
    std::cout << "Successfully created the socket..\n";

//    bind()
    iResult = bind(listenSocket, result -> ai_addr, (int)result -> ai_addrlen);
    if(iResult == SOCKET_ERROR) {
        std::cout << "Binding failed: " << WSAGetLastError() << '\n';
        freeaddrinfo(result);
        exitProgram();
    }
    std::cout << "Socket binding successfully...\n";

    freeaddrinfo(result);
//    listen()
    if(listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listening error: " << WSAGetLastError();
        closesocket(listenSocket);
        exitProgram();
    }
    std::cout<< "Listening...\n";

//    accept()
    SOCKET clientSocket = INVALID_SOCKET;
    for(;;){
        clientSocket = accept(listenSocket, NULL, NULL);
        handle(clientSocket);
        inOut(clientSocket);
    }

    iResult = shutdown(clientSocket, SD_SEND);
    closesocket(clientSocket);
    WSACleanup();

    return EXIT_SUCCESS;
}
