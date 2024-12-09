#include "client.h"
#include <iostream>
#include <vector>

int main()
{
    // Initialize Client
    std::string ip_address = "127.0.0.1";
    int port = 8080;
    wxStaticText *m_statusText = nullptr; // Assuming you have a wxStaticText for status updates
    Client client(ip_address, port, m_statusText);

    // Ask the user if they want to exchange the authorization code for an access token
    char choice;
    std::cout << "Do you want to exchange authorization code for access token? (y/n): ";
    std::cin >> choice;

    if (choice == 'y' || choice == 'Y')
    {
        std::string authorization_code;
        std::cout << "Enter your authorization code: ";
        std::cin >> authorization_code;

        std::string client_id = "926757990224-84lbea6uthpg9kjodd8i9050gr5ie5gl.apps.googleusercontent.com";
        std::string client_secret = "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";
        std::string redirect_uri = "https://localhost:8080";
        GmailReceiver gmailClient(client_id, client_secret, redirect_uri);

        std::string access_token;
        std::string refresh_token;

        // Exchange auth code for access token
        if (gmailClient.exchangeAuthCodeForAccessToken(authorization_code, access_token, refresh_token))
        {
            // Save access token to file
            gmailClient.saveAccessTokenToFile(access_token);
            std::cout << "Access token exchanged and saved successfully!" << std::endl;
        }
        else
        {
            std::cerr << "Failed to exchange authorization code for access token." << std::endl;
            return 1;
        }
    }

    // Load access token from file
    client.loadAccessToken();

    // Initialize client and start checking for messages
    if (!client.initialize(m_statusText))
    {
        std::cerr << "Failed to initialize client." << std::endl;
        return 1;
    }

    return 0;
}