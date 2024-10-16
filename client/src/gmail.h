// gmail.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>

// TODO: Reference additional headers your program requires here.
#include <fstream>
#include <curl/curl.h>
#include "json.hpp"

#include <thread>
#include <chrono>
#include <conio.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
bool exchangeAuthCodeForAccessToken(const std::string& client_id,
    const std::string& client_secret,
    const std::string& authorization_code,
    const std::string& redirect_uri,
    std::string& access_token,
    std::string& refresh_token);
void saveAccessTokenToFile(const std::string& access_token);
bool refreshAccessToken(const std::string& client_id,
    const std::string& client_secret,
    const std::string& refresh_token,
    std::string& new_access_token);
std::string base64UrlDecode(const std::string& input);
void getMessageDetails(const std::string& access_token, const std::string& message_id, std::string& subject, std::string& content);
void markAsRead(const std::string& accessToken, const std::string& messageId);
void getUnreadGmailMessages(const std::string& access_token);
std::vector<std::string> getUnreadMessageContents(const std::string& access_token);
void pollForNewMessages(const std::string& access_token, int interval_seconds);