#pragma once

#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "json.hpp"
#include <thread>
#include <chrono>
#include <conio.h>
#include <vector>

class GmailBase
{
public:
    GmailBase(const std::string &client_id, const std::string &client_secret, const std::string &redirect_url);

    bool exchangeAuthCodeForAccessToken(const std::string &authorization_code,
                                        std::string &access_token,
                                        std::string &refresh_token);
    void saveAccessTokenToFile(const std::string &access_token);
    bool refreshAccessToken(const std::string &refresh_token, std::string &new_access_token);
    static std::string base64UrlDecode(const std::string &input);

protected:
    std::string m_client_id;
    std::string m_client_secret;
    std::string m_redirect_uri;

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
};

class GmailReceiver : public GmailBase
{
public:
    GmailReceiver(const std::string &client_id, const std::string &client_secret, const std::string &redirect_url);

    static void markAsRead(const std::string &access_token, const std::string &message_id);
    static std::string getMessageContent(const std::string &access_token, const std::string &message_id);

    static std::vector<std::string> getUnreadMessageContents(const std::string &access_token);
};

class GmailSender : public GmailBase
{
public:
    GmailSender(const std::string &access_token);

    void setTo(const std::string &to) { m_to = to; }
    void setSubject(const std::string &subject) { m_subject = subject; }
    void setBody(const std::string &body) { m_body = body; }

    bool send();
    std::string getLastError() const { return m_last_error; }

private:
    std::string m_access_token;
    std::string m_to;
    std::string m_subject;
    std::string m_body;
    std::string m_last_error;

    std::string createMimeMessage() const;
    std::string base64UrlEncode(const std::string &input) const;
};