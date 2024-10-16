// client_socket.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>

// TODO: Reference additional headers your program requires here.
#include <fstream>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")  // Link Winsock library


bool initializeWinsock();
SOCKET createClientSocket();
bool connectToServer(SOCKET& clientSocket, const char* ipAddress, int port);
bool receiveFile(SOCKET& serverSocket, const std::wstring& savePath);
void cleanup(SOCKET& socket);
bool sendString(SOCKET& serverSocket, const std::string& message);