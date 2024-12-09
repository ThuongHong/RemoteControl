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
#define MAX_PACKET_SIZE 1400       // Maximum packet size

class Client : public wxEvtHandler
{
public:
    Client(const std::string &ip_address, int port, wxStaticText *m_statusText);
    void loadAccessToken();
    void checkForMessages(wxStaticText *m_statusText);
    bool initialize(wxStaticText *m_statusText);
    void cleanup();
    std::string ip_address_;
    std::string access_token_;
    int port_;

private:
    SOCKET server_socket_;
    wxTimer *m_checkMessageTimer;

    void updateStatus(const wxString &message, wxStaticText *m_statusText);
    void startCheckingMessages(wxStaticText *m_statusText);
    void stopCheckingMessages(wxStaticText *m_statusText);
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