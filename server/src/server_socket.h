// server_socket.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>

// TODO: Reference additional headers your program requires here.
#include <fstream>
#include <string>
#include <vector>

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")  // Link Winsock library

#include <windows.h>

bool initializeWinsock();
SOCKET createServerSocket();
bool bindServerSocket(SOCKET& serverSocket, int port);
bool listenForConnections(SOCKET& serverSocket);
SOCKET acceptClient(SOCKET& serverSocket);
bool sendFile(SOCKET& socket, const std::wstring& filename);
bool receiveString(SOCKET& socket, std::string& receivedMessage);
void cleanup(SOCKET& socket);
