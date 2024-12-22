#include "Client.h"

#ifndef ntohll
#if defined(_MSC_VER)
#define ntohll(x) (((uint64_t)ntohl((uint32_t)((x << 32) >> 32))) << 32) | ntohl((uint32_t)(x >> 32))
#else
#define ntohll(x) be64toh(x)
#endif
#endif

Client::Client(const std::string &ip_address, int port, wxStaticText *m_statusText, std::vector<std::string> &tasks)
    : ip_address_(ip_address), port_(port), server_socket_(INVALID_SOCKET)
{
    m_checkMessageTimer = new wxTimer(this);
    m_checkMessageTimer->Start(3000);
}

void Client::BindControl(wxStaticText *m_statusText, std::string send_email, wxScopedPtr<GmailReceiver> &gmailReceiver)
{
    Bind(wxEVT_TIMER, [this, m_statusText, send_email, &gmailReceiver](wxTimerEvent &)
         { checkForMessage(m_statusText, send_email, gmailReceiver); });
}

void Client::checkForMessage(wxStaticText *m_statusText, std::string send_email, wxScopedPtr<GmailReceiver> &gmailReceiver)
{
    updateStatus("Checking for new messages...", m_statusText);
    std::vector<std::string> tasks = gmailReceiver->getUnreadMessageContentsFromSender(send_email);

    bool stop = false;
    for (const std::string &messageContent : tasks)
    {
        stop = processMessage(messageContent, m_statusText);
        if (stop)
        {
            break;
        }
    }
}

bool Client::initialize(wxStaticText *m_statusText)
{
    if (!initializeSocket())
    {
        updateStatus("Socket initialization failed", m_statusText);
        return false;
    }

    server_socket_ = createClientSocket();
    if (server_socket_ == INVALID_SOCKET)
    {
        updateStatus("Client socket creation failed", m_statusText);
        WSACleanup();
        return false;
    }
    if (connectToServer(ip_address_.c_str(), port_))
    {
        updateStatus("Server connection successfully", m_statusText);
    }
    else
    {
        updateStatus("Server connection failed", m_statusText);
        return false;
    }

    return true;
}

void Client::cleanup()
{
    if (server_socket_ != INVALID_SOCKET)
    {
        cleanupSocket();
    }
}

void Client::updateStatus(const wxString &message, wxStaticText *m_statusText)
{
    if (m_statusText)
    {
        m_statusText->SetLabel(message);
        m_statusText->GetParent()->Layout();
    }
}

bool Client::openFile(const std::wstring &filename)
{
    // Get executable directory
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir = std::wstring(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/"));

    // Build full path
    std::wstring fullPath = exeDir + L"\\received\\" + filename;

    // Check if file exists
    if (!std::filesystem::exists(fullPath))
    {
        std::wcerr << L"File does not exist: " << fullPath << std::endl;
        return false;
    }

    // Open file with full path
    HINSTANCE result = ShellExecute(NULL, L"open", fullPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
    if ((int)result <= 32)
    {
        std::wcerr << L"Failed to open file: " << fullPath << std::endl;
        return false;
    }
    return true;
}

bool Client::processMessage(const std::string &messageContent, wxStaticText *m_statusText)
{
    updateStatus(wxString::Format("Processing: %s", messageContent), m_statusText);

    if (sendString(messageContent))
    {
        std::cout << "\n-----------------------------------\n";
        std::cout << "\nSent successfully!\n";

        if (messageContent.substr(0, 8) == "list app")
        {
            if (receiveFile(L"apps_list.txt"))
            {
                openFile(L"apps_list.txt");
            }
        }
        else if (messageContent.substr(0, 12) == "list service")
        {
            if (receiveFile(L"services_list.txt"))
            {
                openFile(L"services_list.txt");
            }
        }
        else if (messageContent.substr(0, 15) == "take screenshot")
        {
            if (receiveFile(L"screenshot.jpg"))
            {
                openFile(L"screenshot.jpg");
            }
        }
        else if (messageContent.substr(0, 10) == "list files")
        {
            if (receiveFile(L"files_list.txt"))
            {
                openFile(L"files_list.txt");
            }
        }
        else if (messageContent.substr(0, 3) == "get")
        {
            // std::string extension = messageContent.substr(messageContent.find("."));
            // std::wstring wextension(extension.begin(), extension.end());
            std::wstring wfilename = L"received_file.txt";
            if (receiveFile(wfilename))
            {
                openFile(wfilename);
            }
        }
        else if (messageContent.substr(0, 10) == "record cam")
        {
            Sleep(3000);
            if (receiveMessage() != "Record successful")
            {
                std::cerr << "Failed to record webcam." << std::endl;
                return false;
            }
            std::wstring videoFilename = L"recorded_webcam.avi";
            if (receiveFile(videoFilename))
            {
                openFile(videoFilename);
            }
            else
            {
                std::cerr << "Failed to receive video file." << std::endl;
                return false;
            }
        }
        else if (messageContent.substr(0, 11) == "capture cam")
        {
            if (receiveMessage() != "Capture successful")
            {
                std::cerr << "Failed to capture webcam." << std::endl;
                return false;
            }
            std::wstring imageFilename = L"captured_webcam.jpg";
            if (receiveFile(imageFilename))
            {
                openFile(imageFilename);
            }
            else
            {
                std::cerr << "Failed to receive image file." << std::endl;
                return false;
            }
        }
        else if (messageContent.substr(0, 6) == "remove")
        {
            if (!receiveFile(L"files_list.txt"))
            {
                std::cout << "Failed to receive files list" << std::endl;
            }
        }
        else if (messageContent.substr(0, 4) == "kill")
        {
            if (receiveMessage() == "Kill successfully")
            {
                if (!receiveFile(L"apps_list.txt"))
                {
                    std::cout << "Failed to receive apps list" << std::endl;
                }
                if (!receiveFile(L"services_list.txt"))
                {
                    std::cout << "Failed to receive services list" << std::endl;
                }
            }
        }
        else if (messageContent.substr(0, 9) == "start app")
        {
            if (!receiveFile(L"apps_list.txt"))
            {
                std::cout << "Failed to receive apps list" << std::endl;
            }
        }
        else if (messageContent.substr(0, 13) == "start service")
        {
            if (!receiveFile(L"services_list.txt"))
            {
                std::cout << "Failed to receive services list" << std::endl;
            }
        }
        else if (messageContent.substr(0, 3) == "end")
        {
            return true;
        }
    }
    return false;
}

// Socket-related methods
bool Client::initializeSocket()
{
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

SOCKET Client::createClientSocket()
{
    return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

bool Client::connectToServer(const char *ipAddress, int port)
{
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(ipAddress);
    serverAddress.sin_port = htons(port);

    return connect(server_socket_, (sockaddr *)&serverAddress, sizeof(serverAddress)) == 0;
}

bool Client::receiveFile(const std::wstring &filename)
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir = std::wstring(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/"));

    // Create received files directory with full path
    std::wstring dirPath = exeDir + L"\\received";

    if (!std::filesystem::exists(dirPath))
    {
        if (!std::filesystem::create_directories(dirPath))
        {
            std::wcerr << L"Failed to create directory: " << dirPath << std::endl;
            return false;
        }
    }

    // Set full permissions
    std::filesystem::permissions(
        dirPath,
        std::filesystem::perms::owner_all |
            std::filesystem::perms::group_all |
            std::filesystem::perms::others_all,
        std::filesystem::perm_options::replace);

    // Build full path for saving
    std::wstring fullPath = dirPath + L"\\" + filename;

    // Open file with full path
    std::ofstream file(fullPath, std::ios::binary);
    if (!file.is_open())
    {
        std::wcerr << L"Failed to open file: " << fullPath << std::endl;
        return false;
    }

    // Set socket timeout
    DWORD timeout = 5000; // 5 seconds
    setsockopt(server_socket_, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    // Receive file size as uint64_t
    uint64_t fileSize = 0;
    int bytesReceived = recv(server_socket_, reinterpret_cast<char *>(&fileSize), sizeof(fileSize), 0);
    if (bytesReceived != sizeof(fileSize))
    {
        std::cerr << "Error receiving file size. Received " << bytesReceived << " bytes" << std::endl;
        file.close();
        return false;
    }

    fileSize = ntohll(fileSize); // Convert from network byte order
    std::cout << "Expecting file size: " << fileSize << " bytes" << std::endl;

    // Send ACK for file size
    // const char *ack = "ACK";
    // if (send(server_socket_, ack, strlen(ack), 0) <= 0)
    //{
    //    std::cerr << "Failed to send ACK for file size" << std::endl;
    //    file.close();
    //    return false;
    //}

    // Receive file in chunks
    std::vector<char> buffer(MAX_PACKET_SIZE);
    uint64_t totalReceived = 0;
    int lastProgress = -1;

    while (totalReceived < fileSize)
    {
        size_t remaining = fileSize - totalReceived;
        size_t chunkSize = (((remaining) < ((uint64_t)MAX_PACKET_SIZE)) ? (remaining) : ((uint64_t)MAX_PACKET_SIZE));

        bytesReceived = recv(server_socket_, buffer.data(), chunkSize, 0);
        // if (bytesReceived <= 0)
        //{
        //     int error = WSAGetLastError();
        //     if (error == WSAETIMEDOUT)
        //     {
        //         std::cerr << "Connection timed out" << std::endl;
        //     }
        //     else if (error == WSAECONNRESET)
        //     {
        //         std::cerr << "Connection reset by peer" << std::endl;
        //     }
        //     else
        //     {
        //         std::cerr << "Error receiving data: " << error << std::endl;
        //     }

        //    // Try to resend last ACK in case it was lost
        //    send(server_socket_, ack, strlen(ack), 0);
        //    Sleep(1000);
        //    file.close();
        //    return false;
        //}

        file.write(buffer.data(), bytesReceived);
        totalReceived += bytesReceived;

        // Send ACK for chunk
        /*if (send(server_socket_, ack, strlen(ack), 0) <= 0)
        {
            std::cerr << "Failed to send chunk ACK" << std::endl;
            file.close();
            return false;
        }*/

        // Show progress
        int progress = static_cast<int>((totalReceived * 100) / fileSize);
        if (progress != lastProgress)
        {
            std::cout << "\rReceiving file: " << progress << "% ("
                      << totalReceived << "/" << fileSize << " bytes)" << std::flush;
            lastProgress = progress;
        }
    }

    std::cout << "\nFile received successfully: "
              << std::string(filename.begin(), filename.end()) << std::endl;
    file.close();
    return true;
}

void Client::cleanupSocket()
{
    closesocket(server_socket_);
    WSACleanup();
}

bool Client::sendString(const std::string &message)
{
    return send(server_socket_, message.c_str(), message.size(), 0) != SOCKET_ERROR;
}

std::string Client::receiveMessage()
{
    char buffer[1024];
    int bytesReceived = recv(server_socket_, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0)
    {
        return std::string(buffer, bytesReceived);
    }
    return "";
}