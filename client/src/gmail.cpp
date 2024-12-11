#include "gmail.h"
#include "base64.h" // Include a base64 decoding library

using json = nlohmann::json;

// GmailBase implementation
GmailBase::GmailBase(const std::string &client_id, const std::string &client_secret, const std::string &redirect_url)
    : m_client_id(client_id), m_client_secret(client_secret), m_redirect_uri(redirect_url) {}

bool GmailBase::exchangeAuthCodeForAccessToken(const std::string &authorization_code,
                                               std::string &access_token,
                                               std::string &refresh_token)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return false;
    }

    std::string post_fields = "code=" + authorization_code +
                              "&client_id=" + m_client_id +
                              "&client_secret=" + m_client_secret +
                              "&redirect_uri=" + m_redirect_uri +
                              "&grant_type=authorization_code";

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Disable SSL verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Disable SSL verification

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    // std::cout << "Response: " << response << std::endl; // Print the response for debugging

    json json_response = json::parse(response, nullptr, false);
    if (json_response.is_discarded())
    {
        std::cerr << "Failed to parse JSON response" << std::endl;
        return false;
    }

    if (json_response.contains("error"))
    {
        std::cerr << "Error: " << json_response["error"] << std::endl;
        if (json_response.contains("error_description"))
        {
            std::cerr << "Error Description: " << json_response["error_description"] << std::endl;
        }
        return false;
    }

    if (json_response.contains("access_token"))
    {
        access_token = json_response["access_token"];
        if (json_response.contains("refresh_token"))
        {
            refresh_token = json_response["refresh_token"];
        }
        return true;
    }

    std::cerr << "Unexpected response format" << std::endl;
    return false;
}

void GmailBase::saveAccessTokenToFile(const std::string &access_token)
{
    std::ofstream file("access_token.txt");
    if (file.is_open())
    {
        file << access_token;
        file.close();
    }
}

bool GmailBase::refreshAccessToken(const std::string &refresh_token, std::string &new_access_token)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        return false;
    }

    std::string post_fields = "refresh_token=" + refresh_token +
                              "&client_id=" + m_client_id +
                              "&client_secret=" + m_client_secret +
                              "&grant_type=refresh_token";

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        return false;
    }

    json json_response = json::parse(response);
    if (json_response.contains("access_token"))
    {
        new_access_token = json_response["access_token"];
        return true;
    }

    return false;
}

std::string GmailBase::base64UrlDecode(const std::string &input)
{
    std::string base64 = input;
    std::replace(base64.begin(), base64.end(), '-', '+');
    std::replace(base64.begin(), base64.end(), '_', '/');
    switch (base64.size() % 4)
    {
    case 2:
        base64 += "==";
        break;
    case 3:
        base64 += "=";
        break;
    }

    std::string decoded;
    CURL *curl = curl_easy_init();
    if (curl)
    {
        char *output = curl_easy_unescape(curl, base64.c_str(), base64.length(), NULL);
        if (output)
        {
            decoded = std::string(output);
            curl_free(output);
        }
        curl_easy_cleanup(curl);
    }

    return decoded;
}

size_t GmailBase::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// GmailReceiver implementation
GmailReceiver::GmailReceiver(const std::string &client_id, const std::string &client_secret, const std::string &redirect_url)
    : GmailBase(client_id, client_secret, redirect_url) {}

void GmailReceiver::markAsRead(const std::string &access_token, const std::string &message_id)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return;
    }

    std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + message_id + "/modify";
    std::string response;
    struct curl_slist *headers = NULL;
    std::string auth_header = "Authorization: Bearer " + access_token;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string json_data = R"({"removeLabelIds": ["UNREAD"]})";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Disable SSL verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Disable SSL verification

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
    }
    else
    {
        std::cout << "Marked message as read: " << message_id << std::endl;
    }
}

std::string base64url_decode(const std::string &input)
{
    std::string base64 = input;
    std::replace(base64.begin(), base64.end(), '-', '+');
    std::replace(base64.begin(), base64.end(), '_', '/');
    switch (base64.size() % 4)
    {
    case 2:
        base64.append("==");
        break;
    case 3:
        base64.append("=");
        break;
    }
    return base64_decode(base64);
}

std::string GmailReceiver::getMessageContent(const std::string &access_token, const std::string &message_id)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return "";
    }

    std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + message_id;
    std::string response;
    struct curl_slist *headers = NULL;
    std::string auth_header = "Authorization: Bearer " + access_token;
    headers = curl_slist_append(headers, auth_header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Disable SSL verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Disable SSL verification

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        return "";
    }

    json json_response;
    try
    {
        json_response = json::parse(response);
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return "";
    }

    std::string message_content;
    if (json_response.contains("payload") && json_response["payload"].contains("parts"))
    {
        for (const auto &part : json_response["payload"]["parts"])
        {
            if (part.contains("mimeType") && part["mimeType"] == "text/plain" && part.contains("body") && part["body"].contains("data"))
            {
                std::string encoded_content = part["body"]["data"];
                message_content = base64url_decode(encoded_content);
                break;
            }
        }
    }
    else
    {
        std::cerr << "No message content found in the response." << std::endl;
    }

    markAsRead(access_token, message_id);
    
    return message_content;
}

std::vector<std::string> GmailReceiver::getUnreadMessageContents(const std::string &access_token)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return {};
    }

    std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages?q=is:unread";
    std::string response;
    struct curl_slist *headers = NULL;
    std::string auth_header = "Authorization: Bearer " + access_token;
    headers = curl_slist_append(headers, auth_header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Disable SSL verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Disable SSL verification

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        return {};
    }

    json json_response;
    try
    {
        json_response = json::parse(response);
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return {};
    }

    std::vector<std::string> message_contents;
    if (json_response.contains("messages"))
    {
        for (const auto &message : json_response["messages"])
        {
            if (message.contains("id"))
            {
                std::string message_id = message["id"];
                std::string message_content = getMessageContent(access_token, message_id);
                if (!message_content.empty())
                {
                    message_contents.push_back(message_content);
                }
            }
        }
    }
    else
    {
        std::cerr << "No messages found in the response." << std::endl;
    }

    return message_contents;
}

// GmailSender implementation
GmailSender::GmailSender(const std::string &access_token)
    : GmailBase("", "", ""), m_access_token(access_token) {}

std::string GmailSender::createMimeMessage() const
{
    std::stringstream mime;

    mime << "To: " << m_to << "\r\n"
         << "Subject: " << m_subject << "\r\n"
         << "Content-Type: text/plain; charset=utf-8\r\n"
         << "\r\n"
         << m_body;

    return mime.str();
}

bool GmailSender::send()
{
    if (m_to.empty() ||m_body.empty())
    {
        m_last_error = "To, subject and body must not be empty";
        return false;
    }

    CURL *curl = curl_easy_init();
    if (!curl)
    {
        m_last_error = "Failed to initialize CURL";
        return false;
    }

    // Create MIME message and encode it
    std::string mime_message = createMimeMessage();
    std::string encoded_message = base64UrlEncode(mime_message);

    // Prepare JSON payload using nlohmann/json
    json payload = {
        {"raw", encoded_message}};
    std::string json_payload = payload.dump();

    // Set up headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + m_access_token;
    headers = curl_slist_append(headers, auth_header.c_str());

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, "https://gmail.googleapis.com/gmail/v1/users/me/messages/send");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Disable SSL verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Disable SSL verification

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        m_last_error = curl_easy_strerror(res);
        return false;
    }

    std::cout << "Response: " << response << std::endl; // Print the response for debugging

    json json_response = json::parse(response, nullptr, false);
    if (json_response.is_discarded())
    {
        m_last_error = "Failed to parse JSON response";
        return false;
    }

    if (json_response.contains("error"))
    {
        m_last_error = json_response["error"];
        if (json_response.contains("error_description"))
        {
            m_last_error += ": " + json_response["error_description"];
        }
        return false;
    }

    std::cout << "Email sent successfully!" << std::endl;
    return true;
}

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

std::string GmailSender::base64UrlEncode(const std::string &input) const
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    std::string base64;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Ignore newlines
    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    base64.assign(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    std::replace(base64.begin(), base64.end(), '+', '-');
    std::replace(base64.begin(), base64.end(), '/', '_');
    base64.erase(std::remove(base64.begin(), base64.end(), '='), base64.end());

    return base64;
}