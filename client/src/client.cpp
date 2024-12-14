// client.cpp
#include "client.h"

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
}

// void Client::loadAccessToken()
// {
//     std::ifstream token_file("access_token.txt");
//     if (token_file.is_open())
//     {
//         std::getline(token_file, access_token_);
//         token_file.close();
//     }
//     else
//     {
//         std::cerr << "Unable to open access token file." << std::endl;
//     }
// }

// void Client::checkForMessages(wxStaticText *m_statusText)
//{
//     const int interval_seconds = 3; // Polling interval
//     while (true)
//     {
//         updateStatus("Checking for new messages...", m_statusText);
//
//         std::vector<std::string> tasks;
//         try
//         {
//             tasks = m_gmailReceiver.getUnreadMessageContents();
//         }
//         catch (const std::exception &e)
//         {
//             // std::cerr << "Error retrieving messages: " << e.what() << std::endl;
//             updateStatus("Error retrieving messages", m_statusText);
//             break;
//         }
//
//         // if (tasks.empty())
//         // {
//         //     std::cout << "No new messages found." << std::endl;
//         // }
//
//         bool stop = false;
//         for (const std::string &messageContent : tasks)
//         {
//             stop = processMessage(messageContent, m_statusText);
//             if (stop)
//             {
//                 break;
//             }
//         }
//
//         if (stop)
//         {
//             break;
//         }
//
//         updateStatus("Waiting for messages...", m_statusText);
//         std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
//     }
// }
void Client::BindControl(wxStaticText *m_statusText, wxScopedPtr<GmailReceiver> &gmailReceiver)
{
    Bind(wxEVT_TIMER, [this, m_statusText, &gmailReceiver](wxTimerEvent &)
         { checkForMessage(m_statusText, gmailReceiver); });
}

void Client::checkForMessage(wxStaticText *m_statusText, wxScopedPtr<GmailReceiver> &gmailReceiver)
{
    updateStatus("Checking for new messages...", m_statusText);
    std::vector<std::string> tasks = gmailReceiver->getUnreadMessageContents();

    bool stop = false;
    for (const std::string &messageContent : tasks)
    {
        stop = processMessage(messageContent, m_statusText);
        if (stop)
        {
            break;
        }
    }

    updateStatus("Waiting for messages...", m_statusText);
}

// void Client::checkForMessages(wxStaticText* m_statusText)
//{
//     const int interval_seconds = 3; // Polling interval
//     while (true)
//     {
//         updateStatus("Checking for new messages...", m_statusText);
//
//         std::vector<std::string> tasks;
//         try
//         {
//             tasks = m_gmailReceiver.getUnreadMessageContents();
//         }
//         catch (const std::exception& e)
//         {
//             // std::cerr << "Error retrieving messages: " << e.what() << std::endl;
//             updateStatus("Error retrieving messages", m_statusText);
//             break;
//         }
//
//         // if (tasks.empty())
//         // {
//         //     std::cout << "No new messages found." << std::endl;
//         // }
//
//         bool stop = false;
//         for (const std::string& messageContent : tasks)
//         {
//             stop = processMessage(messageContent, m_statusText);
//             if (stop)
//             {
//                 break;
//             }
//         }
//
//         if (stop)
//         {
//             break;
//         }
//
//         updateStatus("Waiting for messages...", m_statusText);
//         std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
//     }
// }

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
        // checkForMessage(m_statusText, tasks);
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

void Client::startCheckingMessages(wxStaticText *m_statusText)
{
    updateStatus("Waiting for message...", m_statusText);
    m_checkMessageTimer->Start(3000);
}

void Client::stopCheckingMessages(wxStaticText *m_statusText)
{
    m_checkMessageTimer->Stop();
    updateStatus("Message checking stopped", m_statusText);
}

bool Client::openFile(const std::wstring &filename)
{
    HINSTANCE result = ShellExecute(NULL, L"open", filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
    if ((int)result <= 32)
    {
        std::cerr << "Failed to open file: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(filename) << std::endl;
        return false;
    }
    return true;
}

bool Client::processMessage(const std::string &messageContent, wxStaticText *m_statusText)
{
    updateStatus(wxString::Format("Processing: %s", messageContent), m_statusText);

    if (sendString(messageContent))
    {
        std::cout << "Sent successfully!\n";
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
            if (receiveFile(L"screenshot.jpeg"))
            {
                openFile(L"screenshot.jpeg");
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
            if (receiveFile(L"get_file"))
            {
                openFile(L"get_file");
            }
        }
        else if (messageContent.substr(0, 9) == "start cam")
        {
            Sleep(6000);
            std::wstring videoFilename = L"received_video.avi";
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
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << std::string(filename.begin(), filename.end()) << std::endl;
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
    const char *ack = "ACK";
    if (send(server_socket_, ack, strlen(ack), 0) <= 0)
    {
        std::cerr << "Failed to send ACK for file size" << std::endl;
        file.close();
        return false;
    }

    // Receive file in chunks
    std::vector<char> buffer(MAX_PACKET_SIZE);
    uint64_t totalReceived = 0;
    int lastProgress = -1;

    while (totalReceived < fileSize)
    {
        size_t remaining = fileSize - totalReceived;
        size_t chunkSize = (((remaining) < ((uint64_t)MAX_PACKET_SIZE)) ? (remaining) : ((uint64_t)MAX_PACKET_SIZE));

        bytesReceived = recv(server_socket_, buffer.data(), chunkSize, 0);
        if (bytesReceived <= 0)
        {
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT)
            {
                std::cerr << "Connection timed out" << std::endl;
            }
            else if (error == WSAECONNRESET)
            {
                std::cerr << "Connection reset by peer" << std::endl;
            }
            else
            {
                std::cerr << "Error receiving data: " << error << std::endl;
            }

            // Try to resend last ACK in case it was lost
            send(server_socket_, ack, strlen(ack), 0);
            Sleep(1000);
            file.close();
            return false;
        }

        file.write(buffer.data(), bytesReceived);
        totalReceived += bytesReceived;

        // Send ACK for chunk
        if (send(server_socket_, ack, strlen(ack), 0) <= 0)
        {
            std::cerr << "Failed to send chunk ACK" << std::endl;
            file.close();
            return false;
        }

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

cv::Mat Client::receiveFrame()
{
    // Receive frame size
    int frameSizeNetworkOrder;
    if (recv(server_socket_, reinterpret_cast<char *>(&frameSizeNetworkOrder), sizeof(frameSizeNetworkOrder), 0) <= 0)
    {
        std::cerr << "Error receiving frame size" << std::endl;
        return cv::Mat();
    }
    int frameSize = ntohl(frameSizeNetworkOrder);

    // Send ACK for frame size
    const char *ack = "ACK";
    if (send(server_socket_, ack, strlen(ack), 0) <= 0)
    {
        std::cerr << "Failed to send ACK for frame size" << std::endl;
        return cv::Mat();
    }

    // Receive frame data in chunks
    std::vector<uchar> buffer(frameSize);
    size_t totalReceived = 0;

    while (totalReceived < frameSize)
    {
        int chunkSize = min(MAX_PACKET_SIZE, frameSize - totalReceived);
        int bytesReceived = recv(server_socket_, reinterpret_cast<char *>(buffer.data() + totalReceived), chunkSize, 0);

        if (bytesReceived <= 0)
        {
            int error = WSAGetLastError();
            if (error == WSAETIMEDOUT)
            {
                std::cerr << "Connection timed out while receiving frame" << std::endl;
            }
            else if (error == WSAECONNRESET)
            {
                std::cerr << "Connection was reset by peer" << std::endl;
            }
            else
            {
                std::cerr << "Error receiving frame data: " << error << std::endl;
            }

            // Try to resend last ACK
            send(server_socket_, ack, strlen(ack), 0);
            return cv::Mat();
        }

        totalReceived += bytesReceived;

        // Send acknowledgment for chunk
        if (send(server_socket_, ack, strlen(ack), 0) <= 0)
        {
            std::cerr << "Failed to send chunk ACK" << std::endl;
            return cv::Mat();
        }
    }

    // Decode frame
    cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
    if (frame.empty())
    {
        std::cerr << "Failed to decode frame" << std::endl;
    }

    return frame;
}