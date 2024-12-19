#include "client.h"
#include "http_listener.h"
#include <iostream>
#include <vector>
#include <curl/curl.h>

bool killProcessOnPort(int port)
{
    std::string cmd = "FOR /F \"tokens=5\" %a IN ('netstat -ano ^| findstr :" +
                      std::to_string(port) + "') DO taskkill /F /PID %a";

    // Execute the command and wait for completion
    int result = system(cmd.c_str());
    if (result == 0)
    {
        // Wait a moment for the port to be freed
        Sleep(1000);
        return true;
    }
    return false;
}

int main(int argc, char** argv)
{
    return wxEntry(argc, argv);
}