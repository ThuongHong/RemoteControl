// client_socket.h
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib") // Link Winsock library

class Socket
{
public:
    static bool initialize();
    static SOCKET createClient();
    static bool connectToServer(SOCKET &clientSocket, const char *ipAddress, int port);
    static bool receiveFile(SOCKET &serverSocket, const std::wstring &savePath);
    static void cleanup(SOCKET &socket);
    static bool sendString(SOCKET &serverSocket, const std::string &message);
};
