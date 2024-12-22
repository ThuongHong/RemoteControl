#include "ServerHandler.h"
#include <iostream>
#include <string>
#include <set>

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
    system("cls");
    std::set<std::string> commands;
    commands.insert("list app");
    commands.insert("list service");
    commands.insert("take screenshot");
    commands.insert("shutdown");
    commands.insert("list files");
    commands.insert("get");
    commands.insert("remove");
    commands.insert("kill");
    commands.insert("start app");
    commands.insert("start service");
    commands.insert("start cam");
    commands.insert("stop cam");
    commands.insert("record cam");
    commands.insert("capture cam");
    commands.insert("end");

    ServerHandler server;

    if (!server.initialize())
    {
        return 1;
    }

    if (!server.bindAndListen(8081))
    {
        std::cerr << "Failed to bind and listen" << std::endl;
        return 1;
    }

    std::cout << "Waiting for client connection on port 8081..." << std::endl;

    SOCKET clientSocket = server.acceptClient();
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to accept client" << std::endl;
        server.cleanup();
        return 1;
    }

    // Get client IP address
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    if (getpeername(clientSocket, (sockaddr *)&clientAddr, &addrLen) == 0)
    {
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        std::cout << "Client " << clientIP << " connected!" << std::endl;
    }
    else
    {
        std::cerr << "Failed to get client IP address" << std::endl;
    }

    // Main server loop
    while (true)
    {
        std::string receivedMessage = server.receiveMessage(clientSocket);
        if (receivedMessage.empty())
            continue;
        bool isCommand = false;
        for (auto &command : commands)
        {
            if (receivedMessage.substr(0, command.size()) == command)
            {
                isCommand = true;
                break;
            }
        }

        if (!isCommand)
        {
            continue;
        }

        std::cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                  << std::endl;
        std::cout << "Received command: " << receivedMessage << std::endl;

        if (receivedMessage.substr(0, 8) == "list app")
        {
            std::vector<ServerHandler::ProcessInfo> applications = server.listRunningApplications();
            server.saveApplicationsToFile(applications, "apps_list.txt");
            if (server.sendFile(clientSocket, L"apps_list.txt"))
            {
                std::cerr << "File sent to client!" << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 12) == "list service")
        {
            std::vector<ServerHandler::ServiceInfo> services = server.listRunningServices();
            server.saveServicesToFile(services, "services_list.txt");
            if (server.sendFile(clientSocket, L"services_list.txt"))
            {
                std::cerr << "File sent to client!" << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 15) == "take screenshot")
        {
            std::wstring filename = L"screenshot.jpg";
            server.takeScreenshot(filename);
            std::wcerr << L"Screenshot saved as " << filename << std::endl;
            if (server.sendFile(clientSocket, filename))
            {
                std::cerr << "File sent to client!" << std::endl;
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
                std::cerr << "File sent to client!" << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 3) == "get")
        {
            std::wstring filename(receivedMessage.begin() + 4, receivedMessage.end());
            filename = trimWString(filename);
            if (server.sendFile(clientSocket, filename))
            {
                std::cerr << "File sent to client!" << std::endl;
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
            std::wstring filelist = L"files_list.txt";
            server.listFilesInDirectory(filelist);
            if (server.sendFile(clientSocket, filelist))
            {
                std::cerr << "File sent to client!" << std::endl;
            }
        }
        else if (receivedMessage.substr(0, 4) == "kill")
        {
            DWORD pid = std::stoul(trimString(std::string(receivedMessage.begin() + 5, receivedMessage.end())));
            if (server.terminateProcessByPID(pid))
            {
                server.sendMessage(clientSocket, "Kill successfully");
                std::cout << "Process terminated successfully" << std::endl;
                // Sleep(1000);

                std::vector<ServerHandler::ProcessInfo> applications = server.listRunningApplications();
                server.saveApplicationsToFile(applications, "apps_list.txt");
                std::vector<ServerHandler::ServiceInfo> services = server.listRunningServices();
                server.saveServicesToFile(services, "services_list.txt");

                if (server.sendFile(clientSocket, L"apps_list.txt"))
                {
                    std::cerr << "File sent to client!" << std::endl;
                    Sleep(3000);
                }
                if (server.sendFile(clientSocket, L"services_list.txt"))
                {
                    std::cerr << "File sent to client!" << std::endl;
                }
            }
        }
        else if (receivedMessage.substr(0, 9) == "start app")
        {
            std::string appName = trimString(std::string(receivedMessage.begin() + 10, receivedMessage.end()));

            if (server.openApplication(appName.c_str()))
                std::cout << "Application " << appName << " started successfully" << std::endl;

            Sleep(2000);
            std::vector<ServerHandler::ProcessInfo> applications = server.listRunningApplications();
            server.saveApplicationsToFile(applications, "apps_list.txt");
            server.sendFile(clientSocket, L"apps_list.txt");
        }

        else if (receivedMessage.substr(0, 13) == "start service")
        {
            std::string serviceName = trimString(std::string(receivedMessage.begin() + 14, receivedMessage.end()));

            if (server.startService(serviceName))
                std::cout << "Service " << serviceName << " started successfully" << std::endl;

            Sleep(2000);
            std::vector<ServerHandler::ServiceInfo> services = server.listRunningServices();
            server.saveServicesToFile(services, "services_list.txt");
            server.sendFile(clientSocket, L"services_list.txt");
        }
        else if (receivedMessage.substr(0, 9) == "start cam")
        {
            server.startWebcam();
        }
        else if (receivedMessage.substr(0, 8) == "stop cam")
        {
            server.stopWebcam();
        }
        else if (receivedMessage.substr(0, 10) == "record cam")
        {
            if (!server.recordWebcam())
            {
                std::cerr << "Failed to record webcam" << std::endl;
                server.sendMessage(clientSocket, "Record failed");
            }
            else
            {
                server.sendMessage(clientSocket, "Record successful");
                std::wstring filename = L"recorded_cam.avi";
                if (server.sendFile(clientSocket, filename))
                {
                    std::cerr << "File sent to client!" << std::endl;
                }
            }
        }
        else if (receivedMessage.substr(0, 11) == "capture cam")
        {
            if (!server.takeWebcamShot())
            {
                std::cerr << "Failed to capture webcam shot" << std::endl;
                server.sendMessage(clientSocket, "Capture failed");
            }
            else
            {
                server.sendMessage(clientSocket, "Capture successful");
                std::wstring filename = L"captured_cam.jpg";
                if (server.sendFile(clientSocket, filename))
                {
                    std::cerr << "File sent to client!" << std::endl;
                }
            }
        }
        else if (receivedMessage.substr(0, 3) == "end")
        {
            break;
        }

        std::cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
                  << std::endl;
    }

    server.cleanup();
    return 0;
}
