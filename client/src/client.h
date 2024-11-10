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

class Client
{
public:
    Client(const std::string &ip_address, int port);
    void run();

    std::string ip_address_;
    int port_;
    SOCKET client_socket_;
    std::string access_token_;

    void loadAccessToken();
    void checkForMessages();
    void cleanup();
};
