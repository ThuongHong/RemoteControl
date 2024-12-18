#include "http_listener.h"
#include <iostream>
#include <sstream>

// http_listener.cpp
HttpListener::HttpListener(int port) : serverSocket(INVALID_SOCKET)
{
    if (!initializeWinsock())
    {
        throw std::runtime_error("Failed to initialize Winsock");
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        throw std::runtime_error("Failed to create socket");
    }

    // Allow socket reuse
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        closesocket(serverSocket);
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR)
    {
        closesocket(serverSocket);
        throw std::runtime_error("Failed to listen on socket");
    }

    std::cout << "HTTP server listening on port " << port << std::endl;
}

HttpListener::~HttpListener()
{
    if (serverSocket != INVALID_SOCKET)
    {
        closesocket(serverSocket);
    }
    WSACleanup();
}

bool HttpListener::initializeWinsock()
{
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

// http_listener.cpp
std::string HttpListener::parseAuthorizationCode(const std::string &request)
{
    // Skip TLS/SSL handshake data and look for HTTP request
    size_t httpGet = request.find("GET");
    if (httpGet == std::string::npos)
    {
        std::cout << "Not an HTTP GET request" << std::endl;
        return "";
    }

    // Extract the request line
    size_t lineEnd = request.find("\r\n", httpGet);
    if (lineEnd == std::string::npos)
    {
        lineEnd = request.find("\n", httpGet);
    }
    if (lineEnd == std::string::npos)
    {
        std::cout << "Malformed HTTP request" << std::endl;
        return "";
    }

    std::string requestLine = request.substr(httpGet, lineEnd - httpGet);
    // std::cout << "Request line: " << requestLine << std::endl;

    // Look for code parameter
    size_t codeStart = requestLine.find("code=");
    if (codeStart == std::string::npos)
    {
        std::cout << "No code parameter found" << std::endl;
        return "";
    }

    codeStart += 5; // Skip "code="
    size_t codeEnd = requestLine.find_first_of("& ", codeStart);
    if (codeEnd == std::string::npos)
    {
        codeEnd = requestLine.length();
    }

    std::string code = requestLine.substr(codeStart, codeEnd - codeStart);
    // std::cout << "Found authorization code: " << code << std::endl;
    return code;
}

std::string HttpListener::waitForAuthorizationCode()
{
    std::cout << "Waiting for client connection..." << std::endl;
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        return "";
    }

    std::string request;
    char buffer[8192];
    int totalBytes = 0;

    // Keep reading until we get a complete HTTP request
    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
            break;

        request.append(buffer, bytesReceived);
        totalBytes += bytesReceived;

        // Look for end of HTTP request
        if (request.find("\r\n\r\n") != std::string::npos)
            break;
    }

    std::cout << "Received " << totalBytes << " bytes" << std::endl;

    // Send success response
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<!DOCTYPE html>\r\n"
        "<html lang='en'>\r\n"
        "<head>\r\n"
        "    <meta charset='UTF-8'>\r\n"
        "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\r\n"
        "    <title>Authorization Success</title>\r\n"
        "    <style>\r\n"
        "        body {\r\n"
        "            font-family: Arial, sans-serif;\r\n"
        "            background-color: #f0f2f5;\r\n"
        "            display: flex;\r\n"
        "            justify-content: center;\r\n"
        "            align-items: center;\r\n"
        "            height: 100vh;\r\n"
        "            margin: 0;\r\n"
        "        }\r\n"
        "        .container {\r\n"
        "            background: white;\r\n"
        "            padding: 40px;\r\n"
        "            border-radius: 10px;\r\n"
        "            box-shadow: 0 2px 10px rgba(0,0,0,0.1);\r\n"
        "            text-align: center;\r\n"
        "        }\r\n"
        "        h1 {\r\n"
        "            color: #1a73e8;\r\n"
        "            margin-bottom: 20px;\r\n"
        "        }\r\n"
        "        p {\r\n"
        "            color: #5f6368;\r\n"
        "            font-size: 18px;\r\n"
        "        }\r\n"
        "        .success-icon {\r\n"
        "            font-size: 64px;\r\n"
        "            margin-bottom: 20px;\r\n"
        "        }\r\n"
        "    </style>\r\n"
        "</head>\r\n"
        "<body>\r\n"
        "    <div class='container'>\r\n"
        "        <div class='success-icon'>✅</div>\r\n"
        "        <h1>Authorization Successful!</h1>\r\n"
        "        <p>Your account has been successfully connected.</p>\r\n"
        "        <p>You can now close this window and return to the application.</p>\r\n"
        "    </div>\r\n"
        "</body>\r\n"
        "</html>";

    send(clientSocket, response.c_str(), response.length(), 0);
    closesocket(clientSocket);

    // Try to parse authorization code
    std::string code = parseAuthorizationCode(request);
    if (!code.empty())
    {
        return code;
    }

    // Continue listening if no code found
    std::cout << "No authorization code in request, continuing to listen..." << std::endl;
}