#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class HttpListener
{
public:
    HttpListener(int port = 8080);
    ~HttpListener();
    std::string waitForAuthorizationCode();

private:
    SOCKET serverSocket;
    bool initializeWinsock();
    std::string parseAuthorizationCode(const std::string &request);
};