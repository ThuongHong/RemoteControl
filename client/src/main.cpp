#include "client.h"
#include "http_listener.h"
#include <iostream>
#include <vector>
#include <curl/curl.h>

bool killProcessOnPort(int port)
{
    std::string cmd = "FOR /F \"tokens=5\" %a IN ('netstat -ano ^| findstr :" +
                      std::to_string(port) + "') DO taskkill /F /PID %a";

    // Execute the command and wait for completion
    int result = system(cmd.c_str());
    if (result == 0)
    {
        // Wait a moment for the port to be freed
        Sleep(1000);
        return true;
    }
    return false;
}

int main()
{
    // Initialize CURL
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return 1;
    }

    // Initialize Client
    std::string ip_address = "127.0.0.1";
    int port = 8081;
    wxStaticText *m_statusText = nullptr;
    Client client(ip_address, port, m_statusText);

    char choice;
    std::cout << "Do you want to exchange authorization code for access token? (y/n): ";
    std::cin >> choice;

    if (choice == 'y' || choice == 'Y')
    {
        try
        {
            std::string client_id = "926757990224-84lbea6uthpg9kjodd8i9050gr5ie5gl.apps.googleusercontent.com";
            std::string client_secret = "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";
            std::string redirect_uri = "http://localhost:8080"; // Changed back to HTTPS to match Google Cloud Console

            // Create Gmail client
            GmailReceiver gmailClient(client_id, client_secret, redirect_uri);

            // Build OAuth2 URL with proper URL encoding
            std::string auth_url =
                "https://accounts.google.com/o/oauth2/v2/auth?"
                "scope=https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fgmail.modify"
                "&access_type=offline"
                "&include_granted_scopes=true"
                "&response_type=code"
                "&redirect_uri=" +
                redirect_uri +
                "&client_id=" + client_id;

            std::cout << "Please visit: " << auth_url << std::endl;

            std::cout << "Waiting for authorization code...\n";

            // Create HTTP listener and wait for authorization code
            try
            {

                HttpListener listener(8080);
                std::string authorization_code = listener.waitForAuthorizationCode();

                if (authorization_code.empty())
                {
                    std::cerr << "Failed to get authorization code\n";
                    curl_global_cleanup();
                    return 1;
                }

                std::cout << "Received authorization code. Exchanging for access token...\n";

                // Exchange authorization code for tokens
                std::string access_token;
                std::string refresh_token;
                if (gmailClient.exchangeAuthCodeForAccessToken(authorization_code, access_token, refresh_token))
                {
                    gmailClient.saveAccessTokenToFile(access_token);
                    std::cout << "Access token exchanged and saved successfully!" << std::endl;
                }
                else
                {
                    std::cerr << "Failed to exchange authorization code for access token." << std::endl;
                    curl_global_cleanup();
                    return 1;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "HTTP Listener error: " << e.what() << std::endl;
                curl_global_cleanup();
                return 1;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            curl_global_cleanup();
            return 1;
        }
    }

    // Load access token from file
    client.loadAccessToken();

    // Initialize client and start checking for messages
    if (!client.initialize(m_statusText))
    {
        std::cerr << "Failed to initialize client." << std::endl;
        curl_global_cleanup();
        return 1;
    }

    curl_global_cleanup();
    return 0;
}