// Client.cpp
#include "client.h"

Client::Client(const std::string &ip_address, int port)
    : ip_address_(ip_address), port_(port), client_socket_(INVALID_SOCKET) {}

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

void Client::checkForMessages()
{
    const int interval_seconds = 3; // Polling interval
    while (true)
    {
        system("cls");

        // Check for ESC key to exit
        if (_kbhit())
        {
            char key = _getch();
            if (key == 27)
            { // ESC key
                std::cout << "ESC key pressed. Exiting..." << std::endl;
                break;
            }
        }

        std::cout << "Checking for new messages..." << std::endl;

        std::vector<std::string> tasks = GmailClient::getUnreadMessageContents(access_token_);
        for (const std::string &messageContent : tasks)
        {
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
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
    }
}

void Client::cleanup()
{
    if (client_socket_ != INVALID_SOCKET)
    {
        Socket::cleanup(client_socket_);
    }
}

void Client::run()
{
    if (!Socket::initialize())
    {
        return;
    }

    client_socket_ = Socket::createClient();
    if (client_socket_ == INVALID_SOCKET)
    {
        WSACleanup();
        return;
    }

    if (Socket::connectToServer(client_socket_, ip_address_.c_str(), port_))
    {
        loadAccessToken();
        checkForMessages();
    }

    cleanup();
}
