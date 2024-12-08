﻿// client_socket.cpp
#include "client_socket.h"
#define min(a, b) (((a) < (b)) ? (a) : (b))
// Function to initialize Winsock
bool Socket::initialize()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
    return true;
}

// Function to create a client socket
SOCKET Socket::createClient()
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
    }
    return sock;
}

// Function to connect to the server
bool Socket::connectToServer(SOCKET &clientSocket, const char *ipAddress, int port)
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress);

    int result = connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return false;
    }
    return true;
}

// Function to receive a file over a socket
bool Socket::receiveFile(SOCKET &serverSocket, const std::wstring &savePath)
{
    std::streamsize fileSize = 0;
    int result = recv(serverSocket, reinterpret_cast<char *>(&fileSize), sizeof(fileSize), 0);
    if (result <= 0)
    {
        std::cerr << "Failed to receive file size: " << WSAGetLastError() << std::endl;
        return false;
    }

    std::vector<char> buffer(fileSize);
    std::streamsize totalReceived = 0;

    // Receive the file data in chunks
    while (totalReceived < fileSize)
    {
        result = recv(serverSocket, buffer.data() + totalReceived, static_cast<int>(fileSize - totalReceived), 0);
        if (result <= 0)
        {
            std::cerr << "File receive failed: " << WSAGetLastError() << std::endl;
            return false;
        }
        totalReceived += result;
    }

    std::ofstream file(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(savePath), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for writing: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(savePath) << std::endl;
        return false;
    }

    file.write(buffer.data(), fileSize);
    file.close();
    std::wcout << L"File received and saved successfully (" << totalReceived << L" bytes)." << std::endl;
    return true;
}

// Function to send a string over a socket
bool Socket::sendString(SOCKET &serverSocket, const std::string &message)
{
    int totalSent = 0;
    int messageLength = message.length();
    const char *data = message.c_str();

    while (totalSent < messageLength)
    {
        int sent = send(serverSocket, data + totalSent, messageLength - totalSent, 0);
        if (sent == SOCKET_ERROR)
        {
            std::cerr << "Failed to send message: " << WSAGetLastError() << std::endl;
            return false;
        }
        totalSent += sent;
    }
    return true;
}

// Function to clean up the socket
void Socket::cleanup(SOCKET &socket)
{
    closesocket(socket);
    WSACleanup();
}

cv::Mat Socket::receiveFrame(SOCKET &serverSocket)
{
    int frameSizeNetworkOrder = 0;
    int result = recv(serverSocket, (char *)&frameSizeNetworkOrder, sizeof(frameSizeNetworkOrder), 0);
    if (result == SOCKET_ERROR || result == 0)
    {
        std::cerr << "Failed to receive frame size: " << WSAGetLastError() << std::endl;
        return cv::Mat();
    }

    int frameSize = ntohl(frameSizeNetworkOrder); // Convert from network byte order

    std::vector<uchar> buffer(frameSize);
    int receivedBytes = 0;

    while (receivedBytes < frameSize)
    {
        int chunkSize = min(MAX_PACKET_SIZE, frameSize - receivedBytes);
        int bytesReceived = recv(serverSocket, (char *)buffer.data() + receivedBytes, chunkSize, 0);
        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            return cv::Mat();
        }
        if (bytesReceived == 0)
        {
            std::cerr << "Connection closed by the server." << std::endl;
            return cv::Mat();
        }
        receivedBytes += bytesReceived;
    }

    // Log buffer content for debugging
    std::cout << "Received frame size: " << frameSize << std::endl;
    std::cout << "Received bytes: " << receivedBytes << std::endl;

    // Check if the buffer is filled correctly
    if (receivedBytes != frameSize)
    {
        std::cerr << "Received bytes do not match frame size." << std::endl;
        return cv::Mat();
    }

    cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
    if (frame.empty())
    {
        std::cerr << "Failed to decode frame." << std::endl;
    }

    return frame;
}