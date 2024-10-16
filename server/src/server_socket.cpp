#include "server_socket.h"

// Function to initialize Winsock
bool initializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
    return true;
}

// Function to create a server socket
SOCKET createServerSocket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
    }
    return sock;
}

// Function to bind the server to an IP and port
bool bindServerSocket(SOCKET& serverSocket, int port) {
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int result = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

// Function to listen for incoming connections
bool listenForConnections(SOCKET& serverSocket) {
    int result = listen(serverSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

// Function to accept a client connection
SOCKET acceptClient(SOCKET& serverSocket) {
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
    }
    return clientSocket;
}

// Function to send a file over a socket
bool sendFile(SOCKET& socket, const std::wstring& filename) {
    // Open the file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << &filename << std::endl;
        return false;  // File couldn't be opened
    }

    // Get the size of the file
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Send the file size first
    if (send(socket, reinterpret_cast<const char*>(&fileSize), sizeof(fileSize), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send file size." << std::endl;
        file.close();
        return false;
    }

    // Send the file data
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        if (send(socket, buffer, file.gcount(), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to send file data." << std::endl;
            file.close();
            return false;
        }
    }

    // Send any remaining bytes
    if (file.gcount() > 0) {
        if (send(socket, buffer, file.gcount(), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to send remaining file data." << std::endl;
            file.close();
            return false;
        }
    }

    file.close();
    return true;  // File sent successfully
}

bool receiveString(SOCKET& socket, std::string& receivedMessage) {
    char buffer[1024];
    int received;

    receivedMessage.clear();

    while ((received = recv(socket, buffer, sizeof(buffer), 0)) > 0) {
        receivedMessage.append(buffer, received);

        // If received < buffer size, it means message is complete.
        if (received < sizeof(buffer)) {
            break;
        }
    }

    if (received == SOCKET_ERROR) {
        std::cerr << "Failed to receive message: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

// Function to clean up the socket
void cleanup(SOCKET& socket) {
    closesocket(socket);
    WSACleanup();
}