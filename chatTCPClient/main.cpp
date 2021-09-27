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

void exitProgram(){
    WSACleanup();
    exit(EXIT_FAILURE);
}

// initialize() -> socket() -> connecT() -> send / recv
int main() {
//    initializing
    WSAData wsaData;
    SOCKET connectSocket = INVALID_SOCKET;
    char recbuffer[MAXIMUM_BUFF], sendbuffer[MAXIMUM_BUFF];
    char username[10];
    char message[150];
    int iSendResult;

//    WSAStartup()
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != 0) {
        std::cout << "Startup failed: " << iResult << "\n";
        exitProgram();
    }
    else{
        std::cout << "WSA Started..\n";
    }

//    getaddrinfo()
    struct addrinfo *result = NULL , *ptr = NULL, hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET; // ipv4
    hints.ai_socktype = SOCK_STREAM; // specify the stream socket.
    hints.ai_protocol = IPPROTO_TCP; // TCP protocol.

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult != 0) {
        std::cout << "getAddrInfo failed: " << iResult << '\n';
        exitProgram();
    }
    std::cout << "AddrInfo successfully.. \n";
//    socket()
    ptr = result;
    connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if(connectSocket == INVALID_SOCKET){
        std::cout << "Error at creating socket \n";
        freeaddrinfo(result);
        exitProgram();
    }
    std::cout << "socket successfully made.. \n";

//    connect()
    iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if(iResult == SOCKET_ERROR){
        std::cout << "Error at connecting..\n";
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if(connectSocket == INVALID_SOCKET){
        std::cout << "Invalid socket error, can t connect to the server.\n";
        closesocket(connectSocket);
        exitProgram();
    }

    std::cout << "Connected to the server!\n";
//    std::cout << "Your username:";
//    std::cin.getline(username, 10);

    std::cout << "Welcome! You can now send messages!\n";
    do{
        strcpy(recbuffer, "");
        strcpy(sendbuffer, "");
        iResult = recv(connectSocket, recbuffer, sizeof(recbuffer), 0);
        if(iResult > 0){
//            PRINT what did you receive from the server.
            for(int i = 0; i < strlen(recbuffer); i++){
                std::cout << recbuffer[i];
            }
            std::cout << "\n";

            std::cin.getline(message, 100);
            strcpy(sendbuffer, message);
            iSendResult = send(connectSocket, sendbuffer, sizeof(sendbuffer), 0);
            if(iSendResult == SOCKET_ERROR) {
                std::cout << "send failed";
                closesocket(connectSocket);
                exitProgram();
            }
        }
        else if (iResult == 0){
            std::cout << "Connection closed\n";
        }
        else{
            std::cout << "Recv failed";
        }
    }while(iResult > 0);

    iResult = shutdown(connectSocket, SD_SEND);
    closesocket(connectSocket);
    WSACleanup();
    return EXIT_SUCCESS;
}
