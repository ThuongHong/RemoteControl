#ifndef WEBCAMSTREAMER_H
#define WEBCAMSTREAMER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define MAX_PACKET_SIZE 1400 // Maximum packet size
#define SERVER_IP "127.0.0.1"

// Function declarations
void initializeWinsock();
SOCKET createSocket();
void sendFrame(SOCKET clientSock, const cv::Mat& frame);
cv::Mat receiveFrame(SOCKET clientSock);

// Server class
class Server {
public:
    Server();
    void start();
    ~Server();

private:
    SOCKET serverSock;
    SOCKET clientSock;
    cv::VideoCapture cap;
    void bindAndListen();
    void acceptClient();
};

// Client class
class Client {
public:
    Client();
    void start();
    ~Client();

private:
    SOCKET clientSock;
    sockaddr_in serverAddr;
    void connectToServer();
};

#endif // WEBCAMSTREAMER_H
