#include "server_socket.h"

// Constructor
ServerSocket::ServerSocket() : serverSocket(INVALID_SOCKET) {}

// Destructor
ServerSocket::~ServerSocket()
{
    cleanup(); // Ensure cleanup on destruction
}

// Function to initialize Winsock
bool ServerSocket::initialize()
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

// Function to create a server socket
SOCKET ServerSocket::create()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
    }
    return serverSocket;
}

// Function to bind the server to an IP and port
bool ServerSocket::bind(int port)
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int result = ::bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

// Function to listen for incoming connections
bool ServerSocket::listenForConnections()
{
    int result = listen(serverSocket, SOMAXCONN);
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

// Function to accept a client connection
SOCKET ServerSocket::acceptClient()
{
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
    }
    return clientSocket;
}

// Function to send a file over a socket
bool ServerSocket::sendFile(SOCKET &clientSocket, const std::wstring &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << &filename << std::endl;
        return false; // File couldn't be opened
    }

    // Get the size of the file
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Send the file size first
    if (send(clientSocket, reinterpret_cast<const char *>(&fileSize), sizeof(fileSize), 0) == SOCKET_ERROR)
    {
        std::cerr << "Failed to send file size." << std::endl;
        file.close();
        return false;
    }

    // Send the file data
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer)))
    {
        if (send(clientSocket, buffer, file.gcount(), 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to send file data." << std::endl;
            file.close();
            return false;
        }
    }

    // Send any remaining bytes
    if (file.gcount() > 0)
    {
        if (send(clientSocket, buffer, file.gcount(), 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to send remaining file data." << std::endl;
            file.close();
            return false;
        }
    }

    file.close();
    return true; // File sent successfully
}

// Function to receive a string from a socket
bool ServerSocket::receiveMessage(SOCKET &clientSocket, std::string &receivedMessage)
{
    char buffer[1024];
    int received;

    receivedMessage.clear();

    while ((received = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
    {
        receivedMessage.append(buffer, received);
        if (received < sizeof(buffer))
        {
            break; // Message is complete
        }
    }

    if (received == SOCKET_ERROR)
    {
        std::cerr << "Failed to receive message: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

// Function to clean up the socket
void ServerSocket::cleanup()
{
    if (serverSocket != INVALID_SOCKET)
    {
        closesocket(serverSocket);
        WSACleanup();
    }
}
