// Client.h
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <thread>
#include <conio.h>
#include "client_socket.h"
#include "gmail.h"
#include <wx/wx.h>

// class Client
//{
// public:
//     Client(const std::string &ip_address, int port);
//     void run();
//
//     std::string ip_address_;
//     int port_;
//     SOCKET client_socket_;
//     std::string access_token_;
//
//     void loadAccessToken();
//     void checkForMessages();
//     void cleanup();
// };

class Client : public wxEvtHandler
{
public:
    Client(std::string &ip_address, int port, wxStaticText *m_statusText);
    void loadAccessToken();
    void checkForMessages(wxStaticText *m_statusText);
    bool initialize(wxStaticText *m_statusText);
    void cleanup();
    std::string ip_address_;
    std::string access_token_;
    int port_;

private:
    SOCKET client_socket_;
    wxTimer *m_checkMessageTimer;

    void updateStatus(const wxString &message, wxStaticText *m_statusText);
    void startCheckingMessages(wxStaticText *m_statusText);
    void stopCheckingMessages(wxStaticText *m_statusText);
    void processMessage(const std::string &messageContent, wxStaticText *m_statusText);
};