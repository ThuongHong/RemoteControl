#include "server_handler.h"
#include <iostream>
#include <string>

std::wstring trimWString(const std::wstring &str)
{
    size_t first = str.find_first_not_of(L" \t\n\r\f\v");
    size_t last = str.find_last_not_of(L" \t\n\r\f\v");
    if (first == std::string::npos)
        return L"";
    return str.substr(first, (last - first + 1));
}

std::string trimString(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    if (first == std::string::npos)
        return "";
    return str.substr(first, (last - first + 1));
}

int main()
{
    ServerHandler server;

    if (!server.initialize())
    {
        return 1;
    }

    if (!server.bindAndListen(8080))
    {
        std::cerr << "Failed to bind and listen" << std::endl;
        return 1;
    }

    std::cout << "Server is listening on port 8080..." << std::endl;

    SOCKET clientSocket = server.acceptClient();
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to accept client" << std::endl;
        server.cleanup();
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    while (true)
    {
        std::string receivedMessage = server.receiveMessage(clientSocket);
        if (receivedMessage.empty())
            continue;

        std::cout << "Received message: " << receivedMessage << std::endl;

        if (receivedMessage.substr(0, 8) == "list app")
        {
            std::cout << "Listing running applications:\n";
            std::vector<ServerHandler::ProcessInfo> applications = server.listRunningApplications();
            server.saveApplicationsToFile(applications, "applications_list.txt");
            if (server.sendFile(clientSocket, L"applications_list.txt"))
            {
                std::cout << "File sent to client!" << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 12) == "list service")
        {
            std::cout << "\nListing running services:\n";
            std::vector<ServerHandler::ServiceInfo> services = server.listRunningServices();
            server.saveServicesToFile(services, "services_list.txt");
            if (server.sendFile(clientSocket, L"services_list.txt"))
            {
                std::cout << "File sent to client!" << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 15) == "take screenshot")
        {
            std::wstring filename = L"screenshot.jpg";
            server.takeScreenshot(filename);
            std::wcerr << L"Screenshot saved as " << filename << std::endl;
            if (server.sendFile(clientSocket, filename))
            {
                std::cout << "File sent to client!" << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 8) == "shutdown")
        {
            std::cout << "Server is shutting down..." << std::endl;
            server.shutdownComputer();
        }
        else if (receivedMessage.substr(0, 10) == "list files")
        {
            std::wstring filename = L"files_list.txt";
            server.listFilesInDirectory(filename);
            if (server.sendFile(clientSocket, filename))
            {
                std::cout << "File sent to client!" << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 3) == "get")
        {
            std::wstring filename(receivedMessage.begin() + 4, receivedMessage.end());
            filename = trimWString(filename);
            if (server.sendFile(clientSocket, filename))
            {
                std::cout << "File sent to client!" << std::endl;
            }
            else
            {
                std::cerr << "Failed to send file to client." << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 6) == "remove")
        {
            std::wstring filename(receivedMessage.begin() + 7, receivedMessage.end());
            filename = trimWString(filename);
            std::wcerr << L"Removing file: " << filename << std::endl;
            if (server.removeFile(filename))
            {
                std::cout << "File successfully removed" << std::endl;
            }
            else
            {
                std::cerr << "Failed to remove file" << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 4) == "kill")
        {
            DWORD pid = std::stoul(trimString(std::string(receivedMessage.begin() + 5, receivedMessage.end())));
            if (server.terminateProcessByPID(pid))
            {
                std::cout << "Process terminated successfully" << std::endl;
            }
            else
            {
                std::cerr << "Failed to terminate process" << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 9) == "start app")
        {
            std::string appName = trimString(std::string(receivedMessage.begin() + 10, receivedMessage.end()));
            char system32Path[MAX_PATH];
            GetSystemDirectoryA(system32Path, MAX_PATH);

            std::string fullPath = std::string(system32Path) + "\\" + appName;
            if (server.openApplication(fullPath.c_str()))
            {
                std::cout << "Application " << appName << " started successfully" << std::endl;
            }
            else
            {
                char windowsPath[MAX_PATH];
                GetWindowsDirectoryA(windowsPath, MAX_PATH);
                fullPath = std::string(windowsPath) + "\\" + appName;

                if (server.openApplication(fullPath.c_str()))
                {
                    std::cout << "Application " << appName << " started successfully" << std::endl;
                }
                else
                {
                    std::cerr << "Failed to start " << appName << std::endl;
                }
            }
        }
        else if (receivedMessage.substr(0, 13) == "start service")
        {
            std::string serviceName = trimString(std::string(receivedMessage.begin() + 14, receivedMessage.end()));
            if (server.startService(serviceName))
            {
                std::cout << "Service " << serviceName << " started successfully" << std::endl;
            }
            else
            {
                std::cerr << "Failed to start service " << serviceName << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 9) == "start cam")
        {
            server.startWebcam(clientSocket);
        }
        else if (receivedMessage.substr(0, 3) == "end")
        {
            break;
        }
    }

    server.cleanup();
    return 0;
}