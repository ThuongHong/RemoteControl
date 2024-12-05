// client.cpp : Defines the entry point for the application.
//

#include "client.h"
#include "App.h"

int main(int argc, char** argv)
{
    return wxEntry(argc, argv);
}
    //std::string ip_address;
    //int port;
    //std::cout << "Enter IP Address: ";
    //std::cin >> ip_address;
    //std::cout << "Enter Port: ";
    //std::cin >> port;

    //Client client(ip_address, port);

    //std::string client_id = "YOUR_CLIENT_ID";
    //std::string client_secret = "YOUR_CLIENT_SECRET";
    //std::string redirect_uri = "YOUR_REDIRECT_URI";
    //std::string access_token;
    //std::string refresh_token;

    //GmailClient gmailClient(client_id, client_secret, redirect_uri);

    //// Ask the user if they want to exchange the authorization code for an access token
    //char choice;
    //std::cout << "Do you want to exchange authorization code for access token? (y/n): ";
    //std::cin >> choice;

    //if (choice == 'y' || choice == 'Y')
    //{
    //    std::string authorization_code;
    //    std::cout << "Enter your authorization code: ";
    //    std::cin >> authorization_code;

    //    // Exchange auth code for access token
    //    if (gmailClient.exchangeAuthCodeForAccessToken(authorization_code, access_token, refresh_token))
    //    {
    //        // Save access token to file
    //        gmailClient.saveAccessTokenToFile(access_token);
    //        std::cout << "Access token exchanged and saved successfully!" << std::endl;
    //    }
    //    else
    //    {
    //        std::cerr << "Failed to exchange authorization code for access token." << std::endl;
    //        return 1;
    //    }
    //}

    //client.loadAccessToken();

    //client.initialize();

    //return 0;
//}
