// client.h
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <thread>
#include <conio.h>
#include <locale>
#include <codecvt>
#include <opencv2/opencv.hpp>
#include <winsock2.h>
#include "gmail.h"
#include <wx/wx.h>

#pragma comment(lib, "Ws2_32.lib") // Link Winsock library

// Constants for buffer management
const size_t MAX_PACKET_SIZE = 1400;            // Maximum packet size
const size_t MAX_BUFFER_SIZE = 8192;            // 8KB chunks
const size_t MAX_FILE_SIZE = 100 * 1024 * 1024; // 100MB limit

class Client : public wxEvtHandler
{
public:
    Client(const std::string &ip_address, int port, wxStaticText *m_statusText, std::vector<std::string> &tasks);
    //void checkForMessages(wxStaticText *m_statusText);
    void checkForMessage(wxStaticText* m_statusText, wxScopedPtr<GmailReceiver> &gmailReceiver);
    bool initialize(wxStaticText *m_statusText);
    void cleanup();
    void BindControl(wxStaticText* m_statusText, wxScopedPtr<GmailReceiver> &gmailReceiver);
    //void startCheckingMessages(wxStaticText *m_statusText);
    std::string ip_address_;
    int port_;

    //GmailReceiver m_gmailReceiver;
    // GmailSender m_gmailSender;
    // void loadAccessToken();
    // std::string access_token_;

private:
    SOCKET server_socket_;
    wxTimer *m_checkMessageTimer;

    void updateStatus(const wxString &message, wxStaticText *m_statusText);
    //void stopCheckingMessages(wxStaticText *m_statusText);
    bool processMessage(const std::string &messageContent, wxStaticText *m_statusText);

    // Socket-related methods
    bool initializeSocket();
    SOCKET createClientSocket();
    bool connectToServer(const char *ipAddress, int port);
    bool receiveFile(const std::wstring &savePath);
    void cleanupSocket();
    bool sendString(const std::string &message);
    cv::Mat receiveFrame();
};