#include "webcamstreamer.h"

// Initialize Winsock
void initializeWinsock() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        exit(1);
    }
}

// Create TCP socket
SOCKET createSocket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        exit(1);
    }
    return sock;
}

// Send frame to client
void sendFrame(SOCKET clientSock, const cv::Mat& frame) {
    std::vector<uchar> buffer;
    cv::imencode(".jpg", frame, buffer); // Compress the frame

    int totalSize = buffer.size();
    send(clientSock, (char*)&totalSize, sizeof(int), 0); // Send frame size first

    int sentBytes = 0;
    while (sentBytes < totalSize) {
        int chunkSize = std::min(MAX_PACKET_SIZE, totalSize - sentBytes);
        int bytesSent = send(clientSock, (char*)buffer.data() + sentBytes, chunkSize, 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            break;
        }
        sentBytes += bytesSent;
    }
}

// Receive and decode the frame
cv::Mat receiveFrame(SOCKET clientSock) {
    int frameSize = 0;
    recv(clientSock, (char*)&frameSize, sizeof(int), 0);

    std::vector<uchar> buffer(frameSize);
    int receivedBytes = 0;

    while (receivedBytes < frameSize) {
        int chunkSize = std::min(MAX_PACKET_SIZE, frameSize - receivedBytes);
        int bytesReceived = recv(clientSock, (char*)buffer.data() + receivedBytes, chunkSize, 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            return cv::Mat();
        }
        receivedBytes += bytesReceived;
    }

    return cv::imdecode(buffer, cv::IMREAD_COLOR); // Decode JPEG data into a frame
}

// Server class implementation

Server::Server() : serverSock(INVALID_SOCKET), clientSock(INVALID_SOCKET) {}

void Server::start() {
    initializeWinsock();
    serverSock = createSocket();
    bindAndListen();
    acceptClient();

    cap.open(0); // Open webcam
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open webcam." << std::endl;
        return;
    }

    while (true) {
        cv::Mat frame;
        cap >> frame; // Capture frame from webcam
        if (frame.empty()) {
            std::cerr << "Error: Could not capture frame." << std::endl;
            break;
        }

        sendFrame(clientSock, frame); // Send frame to client
        cv::imshow("Server - Webcam", frame);

        if (cv::waitKey(1) == 27) {  // ESC to exit
            break;
        }
    }

    closesocket(clientSock);
    closesocket(serverSock);
    WSACleanup();
}

void Server::bindAndListen() {
    sockaddr_in serverAddr = { 0 };
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind and listen for client connections
    if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        exit(1);
    }

    if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        exit(1);
    }

    std::cout << "Server is waiting for a connection..." << std::endl;
}

void Server::acceptClient() {
    clientSock = accept(serverSock, NULL, NULL);
    if (clientSock == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        exit(1);
    }

    std::cout << "Client connected!" << std::endl;
}

Server::~Server() {
    closesocket(clientSock);
    closesocket(serverSock);
}

// Client class implementation

Client::Client() : clientSock(INVALID_SOCKET) {}

void Client::start() {
    initializeWinsock();
    clientSock = createSocket();
    connectToServer();

    while (true) {
        cv::Mat frame = receiveFrame(clientSock); // Receive frame
        if (frame.empty()) {
            std::cerr << "Failed to receive frame." << std::endl;
            break;
        }

        cv::imshow("Client - Webcam Stream", frame);
        if (cv::waitKey(1) == 27) {  // ESC to exit
            break;
        }
    }

    closesocket(clientSock);
    WSACleanup();
}

void Client::connectToServer() {
    sockaddr_in serverAddr = { 0 };
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(clientSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        exit(1);
    }

    std::cout << "Connected to server!" << std::endl;
}

Client::~Client() {
    closesocket(clientSock);
}
