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

bool Client::receiveFile(const std::wstring &savePath)
{
    std::ofstream outputFile(savePath, std::ios::binary);
    if (!outputFile.is_open())
    {
        std::cerr << "Failed to open file for writing: " << savePath << std::endl;
        return false;
    }

    char buffer[MAX_PACKET_SIZE];
    int bytesRead;
    while ((bytesRead = recv(server_socket_, buffer, MAX_PACKET_SIZE, 0)) > 0)
    {
        outputFile.write(buffer, bytesRead);
    }

    outputFile.close();
    return bytesRead == 0;
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