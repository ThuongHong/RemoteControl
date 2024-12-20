#pragma once

#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "json.hpp"
#include <vector>

class OAuth2Handler
{
public:
    OAuth2Handler(const std::string &client_id,
                  const std::string &client_secret,
                  const std::string &redirect_url);

    bool exchangeAuthCodeForAccessToken(const std::string &authorization_code,
                                        std::string &access_token,
                                        std::string &refresh_token);
    static void saveAccessTokenToFile(const std::string &access_token);
    // bool refreshAccessToken(const std::string& refresh_token, std::string& new_access_token);

private:
    std::string m_client_id;
    std::string m_client_secret;
    std::string m_redirect_uri;
};

class GmailReceiver
{
public:
    // explicit GmailReceiver(const std::string &access_token);
    void setAccessToken(const std::string &access_token) { m_access_token = access_token; }
    void markAsRead(const std::string &message_id);
    std::string getMessageContent(const std::string &message_id);
    std::vector<std::string> getUnreadMessageContents();
    std::vector<std::string> getUnreadMessageContentsFromSender(const std::string &sender_email);

private:
    std::string m_access_token;
};

class GmailSender
{
public:
    // explicit GmailSender(const std::string &access_token);
    void setAccessToken(const std::string &access_token) { m_access_token = access_token; }
    void setTo(const std::string &to) { m_to = to; }
    void setSubject(const std::string &subject) { m_subject = subject; }
    void setBody(const std::string &body) { m_body = body; }
    bool send();
    std::string getLastError() const { return m_last_error; }

    std::string m_access_token;
    std::string m_to = "";
    std::string m_subject = "";
    std::string m_body = "";

private:
    std::string m_last_error;
    std::string createMimeMessage() const;
    std::string base64UrlEncode(const std::string &input) const;
};