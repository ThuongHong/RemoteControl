#include "gmail.h"

void DisableSSLVerification(CURL *curl)
{
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Do not verify SSL peer
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Do not verify SSL host
}

// Static callback function for CURL to write response data
size_t GmailClient::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// Constructor
GmailClient::GmailClient(const std::string &client_id, const std::string &client_secret, const std::string &redirect_url)
    : m_client_id(client_id), m_client_secret(client_secret), m_redirect_uri(redirect_url) {}

// Function to exchange auth code for access token
bool GmailClient::exchangeAuthCodeForAccessToken(const std::string &authorization_code,
                                                 std::string &access_token,
                                                 std::string &refresh_token)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    std::string post_data = "code=" + authorization_code +
                            "&client_id=" + m_client_id +
                            "&client_secret=" + m_client_secret +
                            "&redirect_uri=" + m_redirect_uri +
                            "&grant_type=authorization_code";

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Disable SSL verification for debugging (not recommended for production)
        DisableSSLVerification(curl);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }
        nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);
        if (jsonResponse.contains("access_token") && jsonResponse.contains("refresh_token"))
        {
            access_token = jsonResponse["access_token"];
            refresh_token = jsonResponse["refresh_token"];
            curl_easy_cleanup(curl);
            return true;
        }
        else
        {
            std::cerr << "Error: " << jsonResponse.dump() << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }
    }
    return false;
}

// Function to save access token to a file
void GmailClient::saveAccessTokenToFile(const std::string &access_token)
{
    std::ofstream token_file("access_token.txt");
    if (token_file.is_open())
    {
        token_file << access_token;
        token_file.close();
        std::cout << "Access token saved to access_token.txt" << std::endl;
    }
    else
    {
        std::cerr << "Unable to open file to save access token." << std::endl;
    }
}

// Function to refresh the access token
bool GmailClient::refreshAccessToken(const std::string &refresh_token, std::string &new_access_token)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    std::string post_data = "client_id=" + m_client_id +
                            "&client_secret=" + m_client_secret +
                            "&refresh_token=" + refresh_token +
                            "&grant_type=refresh_token";

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        DisableSSLVerification(curl);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }
        nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);
        if (jsonResponse.contains("access_token"))
        {
            new_access_token = jsonResponse["access_token"];
            curl_easy_cleanup(curl);
            return true;
        }
        else
        {
            std::cerr << "Error: " << jsonResponse.dump() << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }
    }
    return false;
}

// Function to decode Base64 URL-encoded content
std::string GmailClient::base64UrlDecode(const std::string &input)
{
    std::string output;
    std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._";

    // Convert Base64 URL encoding to regular Base64 encoding
    std::string base64_input = input;
    std::replace(base64_input.begin(), base64_input.end(), '-', '+');
    std::replace(base64_input.begin(), base64_input.end(), '_', '/');

    // Padding with '=' to make input length a multiple of 4
    while (base64_input.length() % 4)
    {
        base64_input += '=';
    }

    // Decode Base64
    size_t in_len = base64_input.size();
    for (size_t i = 0; i < in_len; i += 4)
    {
        uint32_t temp = (base64_chars.find(base64_input[i]) << 18) +
                        (base64_chars.find(base64_input[i + 1]) << 12) +
                        (base64_chars.find(base64_input[i + 2]) << 6) +
                        base64_chars.find(base64_input[i + 3]);
        if (base64_input[i + 2] != '=')
            output.push_back((temp >> 16) & 0xFF);
        if (base64_input[i + 3] != '=')
            output.push_back((temp >> 8) & 0xFF);
        if (base64_input[i + 3] != '=' && base64_input[i + 2] != '=')
            output.push_back(temp & 0xFF);
    }

    return output;
}

// Function to get details of a message by ID
void GmailClient::getMessageDetails(const std::string &access_token, const std::string &message_id,
                                    std::string &subject, std::string &content)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl)
    {
        // Set the URL for fetching a specific message by ID
        std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + message_id;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the authorization header
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        DisableSSLVerification(curl);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // Parse the response as JSON
            nlohmann::json messageResponse = nlohmann::json::parse(readBuffer);

            // Extract subject from headers
            for (const auto &header : messageResponse["payload"]["headers"])
            {
                if (header["name"] == "Subject")
                {
                    subject = header["value"];
                }
            }

            // Get message body (if present)
            if (messageResponse["payload"].contains("parts"))
            {
                for (const auto &part : messageResponse["payload"]["parts"])
                {
                    if (part["mimeType"] == "text/plain")
                    {
                        content = part["body"]["data"];     // Base64url encoded content
                        content = base64UrlDecode(content); // Decode the Base64 content
                        break;                              // Exit after the first plain text part
                    }
                }
            }
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

// Function to mark a message as read
void GmailClient::markAsRead(const std::string &accessToken, const std::string &messageId)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    // Initialize curl
    curl = curl_easy_init();
    if (curl)
    {
        // Set the URL for the API endpoint
        std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages/" + messageId + "/modify";

        // Prepare the JSON payload
        std::string jsonPayload = R"({"removeLabelIds": ["UNREAD"]})";

        // Set curl options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

        // Set headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        DisableSSLVerification(curl);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

std::vector<std::string> GmailClient::getUnreadMessageContents(const std::string &access_token)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::vector<std::string> messageContents;

    curl = curl_easy_init();
    if (curl)
    {
        std::string url = "https://gmail.googleapis.com/gmail/v1/users/me/messages?q=is:unread";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        DisableSSLVerification(curl);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            nlohmann::json response = nlohmann::json::parse(readBuffer);
            if (response.contains("messages"))
            {
                for (const auto &message : response["messages"])
                {
                    std::string message_id = message["id"];
                    std::string subject, content;
                    getMessageDetails(access_token, message_id, subject, content);
                    messageContents.push_back(content);

                    // Mark the message as read
                    markAsRead(access_token, message_id);
                }
            }
        }
        else
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return messageContents;
}

void GmailClient::pollForNewMessages(const std::string &access_token, int interval_seconds)
{
    while (true)
    {
        std::vector<std::string> newMessages = getUnreadMessageContents(access_token);
        for (const auto &message : newMessages)
        {
            std::cout << "New Message: " << message << std::endl;
        }

        // Wait for the specified interval
        std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
    }
}
