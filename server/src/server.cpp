#include "server_socket.h"
#include "server_task.h"

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
    ServerSocket serverSocket;
    if (!serverSocket.initialize())
    {
        return 1;
    }

    if (serverSocket.create() == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    if (serverSocket.bind(8080) && serverSocket.listenForConnections())
    {
        std::cout << "Server is listening on port 8080..." << std::endl;

        SOCKET clientSocket = serverSocket.acceptClient();
        if (clientSocket != INVALID_SOCKET)
        {
            std::cout << "Client connected!" << std::endl;

            ServerTask serverTask;
            while (true)
            {
                std::string receivedMessage;
                if (serverSocket.receiveMessage(clientSocket, receivedMessage))
                {
                    std::cout << "Received message: " << receivedMessage << std::endl;

                    if (receivedMessage.substr(0, 8) == "list app")
                    {
                        std::cout << "Listing running applications:\n";
                        std::vector<ServerTask::ProcessInfo> applications = serverTask.listRunningApplications();
                        serverTask.saveApplicationsToFile(applications, "applications_list.txt");
                        if (serverSocket.sendFile(clientSocket, L"applications_list.txt"))
                            std::cout << "File sent to client!" << std::endl;
                    }
                    else if (receivedMessage.substr(0, 12) == "list service")
                    {
                        std::cout << "\nListing running services:\n";
                        std::vector<ServerTask::ServiceInfo> services = serverTask.listRunningServices();
                        serverTask.saveServicesToFile(services, "services_list.txt");
                        if (serverSocket.sendFile(clientSocket, L"services_list.txt"))
                            std::cout << "File sent to client!" << std::endl;
                    }
                    else if (receivedMessage.substr(0, 15) == "take screenshot")
                    {
                        std::wstring filename = L"screenshot.jpg";
                        serverTask.takeScreenshot(filename);
                        std::wcerr << "Screenshot saved as " << filename << std::endl;
                        if (serverSocket.sendFile(clientSocket, filename))
                        {
                            std::cout << "File sent to client!" << std::endl;
                        }
                    }
                    else if (receivedMessage.substr(0, 8) == "shutdown")
                    {
                        std::cout << "Server is shutting down..." << std::endl;
                        serverTask.shutdownComputer();
                    }
                    else if (receivedMessage.substr(0, 10) == "list files")
                    {
                        std::wstring filename = L"files_list.txt";
                        serverTask.listFilesInDirectory(filename);
                        if (serverSocket.sendFile(clientSocket, filename))
                        {
                            std::cout << "File sent to client!" << std::endl;
                        }
                    }
                    else if (receivedMessage.substr(0, 3) == "get")
                    {
                        std::wstring filename = std::wstring(receivedMessage.begin() + 4, receivedMessage.end());
                        filename = trimWString(filename);
                        if (serverSocket.sendFile(clientSocket, filename))
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
                        std::wcerr << "Removing file: " << filename << std::endl;
                        // return 0;
                        if (serverTask.removeFile(filename))
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
                        if (serverTask.terminateProcessByPID(pid))
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

                        // Get System32 directory
                        char system32Path[MAX_PATH];
                        GetSystemDirectoryA(system32Path, MAX_PATH);

                        // Try different paths
                        std::string fullPath = std::string(system32Path) + "\\" + appName;

                        if (serverTask.openApplication(fullPath.c_str()))
                        {
                            std::cout << "Application " << appName << " started successfully" << std::endl;
                        }
                        else
                        {
                            // Try Windows directory if System32 failed
                            char windowsPath[MAX_PATH];
                            GetWindowsDirectoryA(windowsPath, MAX_PATH);
                            fullPath = std::string(windowsPath) + "\\" + appName;

                            if (serverTask.openApplication(fullPath.c_str()))
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

                        if (serverTask.startService(serviceName))
                        {
                            std::cout << "Service " << serviceName << " started successfully" << std::endl;
                        }
                        else
                        {
                            std::cerr << "Failed to start service " << serviceName << std::endl;
                        }
                    }

                    else if (receivedMessage == "end")
                    {
                        break;
                    }
                }
            }
        }

        serverSocket.cleanup();
        return 0;
    }
}