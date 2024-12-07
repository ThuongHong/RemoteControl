#include "server_socket.h"
#include "server_task.h"

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
                    else if (receivedMessage == "end")
                    {
                        break;
                    }
                }
            }
        }
    }

    serverSocket.cleanup();
    return 0;
}
