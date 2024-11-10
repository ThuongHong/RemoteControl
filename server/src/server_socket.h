#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#include <Gdiplus.h>

class ServerSocket
{
public:
    ServerSocket();  // Constructor
    ~ServerSocket(); // Destructor

    bool initialize();                                                       // Initializes Winsock
    SOCKET create();                                                         // Creates a server socket
    bool bind(int port);                                                     // Binds the server socket to an IP and port
    bool listenForConnections();                                             // Listens for incoming connections
    SOCKET acceptClient();                                                   // Accepts a client connection
    bool sendFile(SOCKET &clientSocket, const std::wstring &filename);       // Sends a file
    bool receiveMessage(SOCKET &clientSocket, std::string &receivedMessage); // Receives a string
    void cleanup();                                                          // Cleans up the socket

private:
    SOCKET serverSocket; // The server socket handle
};

#endif // SERVER_SOCKET_H
