#pragma once

#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "json.hpp"
#include <thread>
#include <chrono>
#include <conio.h>
#include <vector>

class GmailClient
{
public:
    // Constructor
    GmailClient(const std::string &client_id, const std::string &client_secret, const std::string &redirect_url);

    // Non-static methods
    bool exchangeAuthCodeForAccessToken(const std::string &authorization_code,
                                        std::string &access_token,
                                        std::string &refresh_token);
    void saveAccessTokenToFile(const std::string &access_token);
    bool refreshAccessToken(const std::string &refresh_token, std::string &new_access_token);
    static std::string base64UrlDecode(const std::string &input);
    static void getMessageDetails(const std::string &access_token, const std::string &message_id,
                                  std::string &subject, std::string &content);
    static void markAsRead(const std::string &access_token, const std::string &message_id);

    // Static methods for polling and getting unread messages
    static std::vector<std::string> getUnreadMessageContents(const std::string &access_token);
    static void pollForNewMessages(const std::string &access_token, int interval_seconds);

    std::string m_client_id;
    std::string m_client_secret;
    std::string m_redirect_uri;

private:
    // Callback function for CURL
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
};
