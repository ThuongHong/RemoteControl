// client_socket.cpp : Defines the entry point for the application.
//

#include "client_socket.h"


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


// Function to create a client socket
SOCKET createClientSocket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
    }
    return sock;
}


// Function to connect to the server
bool connectToServer(SOCKET& clientSocket, const char* ipAddress, int port) {
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress);

    int result = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return false;
    }
    return true;
}


// Function to receive a file over a socket
bool receiveFile(SOCKET& serverSocket, const std::wstring& savePath) {
    // Receive the file size
    std::streamsize fileSize = 0;
    int result = recv(serverSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
    if (result <= 0) {
        std::cerr << "Failed to receive file size: " << WSAGetLastError() << std::endl;
        return false;
    }

    std::vector<char> buffer(fileSize);
    std::streamsize totalReceived = 0;

    // Receive the file data in chunks
    while (totalReceived < fileSize) {
        result = recv(serverSocket, buffer.data() + totalReceived, static_cast<int>(fileSize - totalReceived), 0);
        if (result <= 0) {
            std::cerr << "File receive failed: " << WSAGetLastError() << std::endl;
            return false;
        }
        totalReceived += result;
    }

    // Save the file to disk
    std::ofstream file(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(savePath), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(savePath) << std::endl;
        return false;
    }

    file.write(buffer.data(), fileSize);
    file.close();

    std::wcout << L"File received and saved successfully (" << totalReceived << L" bytes)." << std::endl;
    return true;
}


bool sendString(SOCKET& serverSocket, const std::string& message) {
    int totalSent = 0;
    int messageLength = message.length();
    const char* data = message.c_str();

    while (totalSent < messageLength) {
        int sent = send(serverSocket, data + totalSent, messageLength - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "Failed to send message: " << WSAGetLastError() << std::endl;
            return false;
        }
        totalSent += sent;
    }
    return true;
}


// Function to clean up the socket
void cleanup(SOCKET& socket) {
    closesocket(socket);
    WSACleanup();
}

//int main() {
    //if (!initializeWinsock()) {
    //    return 1;
    //}

    //SOCKET clientSocket = createClientSocket();
    //if (clientSocket == INVALID_SOCKET) {
    //    WSACleanup();
    //    return 1;
    //}

    //std::wstring filename = L"3.jpg";
    //if (connectToServer(clientSocket, "127.0.0.1", 8080)) {
    //    receiveFile(clientSocket, filename);  // Save as this file
    //}

    //cleanup(clientSocket);
//
//    return 0;
//}
