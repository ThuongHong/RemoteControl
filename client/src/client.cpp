// Client.cpp
#include "client.h"

// Client::Client(const std::string &ip_address, int port)
//     : ip_address_(ip_address), port_(port), client_socket_(INVALID_SOCKET) {}
//
// void Client::loadAccessToken()
//{
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
//
// void Client::checkForMessages()
//{
//     const int interval_seconds = 3; // Polling interval
//     while (true)
//     {
//         system("cls");
//
//         // Check for ESC key to exit
//         if (_kbhit())
//         {
//             char key = _getch();
//             if (key == 27)
//             { // ESC key
//                 std::cout << "ESC key pressed. Exiting..." << std::endl;
//                 break;
//             }
//         }
//
//         std::cout << "Checking for new messages..." << std::endl;
//
//         std::vector<std::string> tasks = GmailClient::getUnreadMessageContents(access_token_);
//         for (const std::string &messageContent : tasks)
//         {
//             if (Socket::sendString(client_socket_, messageContent))
//             {
//                 std::cout << "Sent successfully!\n";
//                 if (messageContent.substr(0, 8) == "list app")
//                 {
//                     Socket::receiveFile(client_socket_, L"apps_list.txt");
//                 }
//                 else if (messageContent.substr(0, 12) == "list service")
//                 {
//                     Socket::receiveFile(client_socket_, L"services_list.txt");
//                 }
//                 else if (messageContent.substr(0, 15) == "take screenshot")
//                 {
//                     Socket::receiveFile(client_socket_, L"screenshot.jpeg");
//                 }
//             }
//         }
//
//         std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
//     }
// }
//
// void Client::cleanup()
//{
//     if (client_socket_ != INVALID_SOCKET)
//     {
//         Socket::cleanup(client_socket_);
//     }
// }
//
// void Client::run()
// {
//     if (!Socket::initialize())
//     {
//         return;
//     }

//     client_socket_ = Socket::createClient();
//     if (client_socket_ == INVALID_SOCKET)
//     {
//         WSACleanup();
//         return;
//     }

//     if (Socket::connectToServer(client_socket_, ip_address_.c_str(), port_))
//     {
//         loadAccessToken();
//         checkForMessages();
//     }

//     cleanup();
// }

Client::Client(std::string &ip_address, int port, wxStaticText *m_statusText)
    : ip_address_(ip_address), port_(port), client_socket_(INVALID_SOCKET)
{
    m_checkMessageTimer = new wxTimer(this);
    // Bind(wxEVT_TIMER, &Client::checkForMessages, this);
    Bind(wxEVT_TIMER, [this, &m_statusText](wxTimerEvent &)
         { checkForMessages(m_statusText); });

    // ButtonConfirm->Bind(wxEVT_BUTTON, [this, desPanel1, desPanel2, &ip_address, &port, &target_email, m_statusText, &client](wxCommandEvent&) {
    //     OnButtonClicked(desPanel1, desPanel2, ip_address, port, target_email, m_statusText, client);
    //     });
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
        std::vector<std::string> tasks = GmailClient::getUnreadMessageContents(access_token_);

        for (const std::string &messageContent : tasks)
        {
            processMessage(messageContent, m_statusText);
        }

        updateStatus("Waiting for messages...", m_statusText);

        std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
    }
    // try
    // {
    //     const int interval_seconds = 3; // Polling interval
    //     while (true)
    //     {
    //         updateStatus("Checking for new messages...", m_statusText);
    //         std::vector<std::string> tasks = GmailClient::getUnreadMessageContents(access_token_);

    //         for (const std::string &messageContent : tasks)
    //         {
    //             processMessage(messageContent, m_statusText);
    //         }

    //         updateStatus("Waiting for messages...", m_statusText);

    //         std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
    //     }
    // }
    // catch (const std::exception &e)
    // {
    //     updateStatus(wxString::Format("Error: %s", e.what()), m_statusText);
    // }
}

bool Client::initialize(wxStaticText *m_statusText)
{
    if (!Socket::initialize())
    {
        updateStatus("Socket initialization failed", m_statusText);
        return false;
    }

    client_socket_ = Socket::createClient();
    if (client_socket_ == INVALID_SOCKET)
    {
        updateStatus("Client socket creation failed", m_statusText);
        WSACleanup();
        return false;
    }

    if (Socket::connectToServer(client_socket_, ip_address_.c_str(), port_))
    {
        loadAccessToken();
        checkForMessages(m_statusText);
    }

    updateStatus("Server connection failed", m_statusText);
    return false;
}

void Client::cleanup()
{
    if (client_socket_ != INVALID_SOCKET)
    {
        Socket::cleanup(client_socket_);
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

void Client::processMessage(const std::string &messageContent, wxStaticText *m_statusText)
{
    updateStatus(wxString::Format("Processing: %s", messageContent), m_statusText);

    if (Socket::sendString(client_socket_, messageContent))
    {
        std::cout << "Sent successfully!\n";
        if (messageContent.substr(0, 8) == "list app")
        {
            Socket::receiveFile(client_socket_, L"apps_list.txt");
        }
        else if (messageContent.substr(0, 12) == "list service")
        {
            Socket::receiveFile(client_socket_, L"services_list.txt");
        }
        else if (messageContent.substr(0, 15) == "take screenshot")
        {
            Socket::receiveFile(client_socket_, L"screenshot.jpeg");
        }
        else if (messageContent.substr(0, 10) == "list files")
        {
            Socket::receiveFile(client_socket_, L"files_list.txt");
        }
        else if (messageContent.substr(0, 3) == "get")
        {
            // std::wstring file_name = std::wstring(messageContent.begin() + 4, messageContent.end());
            Socket::receiveFile(client_socket_, L"get_file");
        }
    }
}
