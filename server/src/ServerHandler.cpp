// server_handler.cpp

#include "ServerHandler.h"
#include <locale>
#include <codecvt>
#include <Shellapi.h>

// Constructor
ServerHandler::ServerHandler() : serverSocket(INVALID_SOCKET),
                                 initialized(false),
                                 stopWebcamFlag(false)
{
    initializeGdiplus();
}

// Destructor
ServerHandler::~ServerHandler()
{
    cleanup();
}

// Socket Methods
bool ServerHandler::initialize()
{
    if (initialized)
        return true;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return false;
    }

    initialized = true;
    return true;
}

bool ServerHandler::bindAndListen(int port)
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed" << std::endl;
        cleanup();
        return false;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed" << std::endl;
        cleanup();
        return false;
    }

    return true;
}

SOCKET ServerHandler::acceptClient()
{
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Accept failed" << std::endl;
        return INVALID_SOCKET;
    }
    return clientSocket;
}

bool ServerHandler::sendMessage(SOCKET clientSocket, const std::string &message)
{
    if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR)
    {
        std::cerr << "Send failed" << std::endl;
        return false;
    }
    return true;
}

bool ServerHandler::sendFile(SOCKET clientSocket, const std::wstring &filename)
{
    // Get executable directory
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir = std::wstring(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/"));

    // Create full path
    std::wstring fullPath = exeDir + L"\\" + filename;
    
    // Check if file exists
    if (!std::filesystem::exists(fullPath))
    {
        std::wcerr << L"File does not exist: " << fullPath << std::endl;
        return false;
    }

    // Open file
    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open())
    {
        DWORD error = GetLastError();
        std::wcerr << L"Failed to open file: " << fullPath << std::endl;
        std::cerr << "Error code: " << error << std::endl;
        return false;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    uint64_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::wcout << L"Sending file: " << fullPath << std::endl;
    std::cout << "File size: " << fileSize << " bytes" << std::endl;

    // Send file size
    uint64_t fileSizeNetwork = htonll(fileSize);
    if (send(clientSocket, reinterpret_cast<char*>(&fileSizeNetwork), sizeof(fileSizeNetwork), 0) != sizeof(fileSizeNetwork))
    {
        std::cerr << "Error sending file size" << std::endl;
        file.close();
        return false;
    }

    // Wait for acknowledgment
    char ack[4];
    int retryCount = 0;
    const int maxRetries = 5;
    const int retryDelay = 1000;
    const size_t chunkSize = 8192;

    while (retryCount < maxRetries)
    {
        int ackReceived = recv(clientSocket, ack, sizeof(ack), 0);
        if (ackReceived > 0 && std::string(ack, ackReceived) == "ACK")
            break;
        Sleep(retryDelay);
        if (++retryCount >= maxRetries)
        {
            std::cerr << "Failed to receive initial acknowledgment" << std::endl;
            file.close();
            return false;
        }
    }

    // Send file in chunks
    std::vector<char> buffer(chunkSize);
    uint64_t totalSent = 0;

    while (totalSent < fileSize)
    {
        size_t remaining = fileSize - totalSent;
        size_t bytesToRead = std::min(remaining, static_cast<uint64_t>(chunkSize));
        
        file.read(buffer.data(), bytesToRead);
        size_t bytesRead = file.gcount();

        int bytesSent = send(clientSocket, buffer.data(), bytesRead, 0);
        if (bytesSent <= 0)
        {
            std::cerr << "Error sending data. WSA Error: " << WSAGetLastError() << std::endl;
            file.close();
            return false;
        }

        totalSent += bytesSent;

        // Wait for chunk acknowledgment
        if (recv(clientSocket, ack, sizeof(ack), 0) <= 0)
        {
            std::cerr << "Failed to receive chunk acknowledgment" << std::endl;
            file.close();
            return false;
        }

        // Show progress
        int percentComplete = static_cast<int>((totalSent * 100) / fileSize);
        std::cout << "\rProgress: " << percentComplete << "% (" 
                  << totalSent << "/" << fileSize << " bytes)" << std::flush;
    }

    std::cout << "\nFile sent successfully: " << totalSent << " bytes" << std::endl;
    file.close();
    return true;
}

// bool ServerHandler::sendFile(SOCKET clientSocket, const std::wstring &filename)
// {
//     std::ifstream file(filename, std::ios::binary);
//     if (!file.is_open())
//     {
//         std::cerr << "Failed to open file: " << std::string(filename.begin(), filename.end()) << std::endl;
//         return false;
//     }

//     // Get file size
//     file.seekg(0, std::ios::end);
//     size_t fileSize = file.tellg();
//     file.seekg(0, std::ios::beg);

//     // Send file size first
//     size_t fileSizeNetworkOrder = htonl(fileSize);
//     if (send(clientSocket, reinterpret_cast<char *>(&fileSizeNetworkOrder), sizeof(fileSizeNetworkOrder), 0) == SOCKET_ERROR)
//     {
//         std::cerr << "Error sending file size" << std::endl;
//         file.close();
//         return false;
//     }

//     // Send file data in chunks
//     const size_t chunkSize = MAX_PACKET_SIZE;
//     std::vector<char> buffer(chunkSize);
//     size_t totalSent = 0;

//     while (totalSent < fileSize)
//     {
//         file.read(buffer.data(), chunkSize);
//         size_t bytesToSend = file.gcount();

//         int bytesSent = send(clientSocket, buffer.data(), bytesToSend, 0);
//         if (bytesSent == SOCKET_ERROR)
//         {
//             std::cerr << "Error sending file data" << std::endl;
//             file.close();
//             return false;
//         }

//         totalSent += bytesSent;

//         // Wait for acknowledgment from client
//         char ack[4];
//         int ackReceived = recv(clientSocket, ack, sizeof(ack), 0);
//         if (ackReceived <= 0 || std::string(ack, ackReceived) != "ACK")
//         {
//             std::cerr << "Failed to receive acknowledgment from client" << std::endl;
//             file.close();
//             return false;
//         }

//         // Show progress
//         int percentComplete = static_cast<int>((totalSent * 100) / fileSize);
//         std::cout << "\rSending file: " << percentComplete << "%" << std::flush;
//     }

//     std::cout << "\nFile sent successfully" << std::endl;
//     file.close();
//     return true;
// }

// bool ServerHandler::sendFrame(SOCKET clientSocket, cv::Mat &frame)
// {
//     try
//     {
//         if (frame.empty() || clientSocket == INVALID_SOCKET)
//         {
//             std::cerr << "Invalid frame or socket" << std::endl;
//             return false;
//         }

//         // Encode frame
//         std::vector<uchar> buffer;
//         if (!cv::imencode(".jpg", frame, buffer, {cv::IMWRITE_JPEG_QUALITY, 80}))
//         {
//             std::cerr << "Failed to encode frame" << std::endl;
//             return false;
//         }

//         // Send frame size
//         int frameSize = static_cast<int>(buffer.size());
//         int frameSizeNetworkOrder = htonl(frameSize);

//         // Wait for socket to be ready
//         fd_set writeSet;
//         struct timeval timeout;
//         timeout.tv_sec = 5; // Increase timeout to 5 seconds
//         timeout.tv_usec = 0;

//         FD_ZERO(&writeSet);
//         FD_SET(clientSocket, &writeSet);

//         if (select(0, NULL, &writeSet, NULL, &timeout) > 0)
//         {
//             if (send(clientSocket, reinterpret_cast<char *>(&frameSizeNetworkOrder),
//                      sizeof(frameSizeNetworkOrder), 0) == SOCKET_ERROR)
//             {
//                 if (WSAGetLastError() != WSAEWOULDBLOCK)
//                 {
//                     std::cerr << "Error sending frame size: " << WSAGetLastError() << std::endl;
//                     return false;
//                 }
//             }
//         }

//         // Wait for acknowledgment from client with retry
//         char ack[4];
//         int ackReceived = 0;
//         int retryCount = 0;
//         const int maxRetries = 5;

//         while (retryCount < maxRetries)
//         {
//             ackReceived = recv(clientSocket, ack, sizeof(ack), 0);
//             if (ackReceived > 0 && std::string(ack, ackReceived) == "ACK")
//             {
//                 break;
//             }
//             else
//             {
//                 retryCount++;
//                 Sleep(1000); // Wait 1 second before retrying
//             }
//         }

//         if (retryCount >= maxRetries)
//         {
//             std::cerr << "Failed to receive acknowledgment for frame size from client" << std::endl;
//             return false;
//         }

//         // Send frame data in chunks with retry
//         const size_t chunkSize = MAX_PACKET_SIZE;
//         size_t bytesSent = 0;
//         retryCount = 0;

//         while (bytesSent < buffer.size() && retryCount < maxRetries)
//         {
//             size_t remainingBytes = buffer.size() - bytesSent;
//             size_t bytesToSend = std::min(remainingBytes, chunkSize);

//             // Wait for socket to be ready
//             FD_ZERO(&writeSet);
//             FD_SET(clientSocket, &writeSet);

//             if (select(0, NULL, &writeSet, NULL, &timeout) > 0)
//             {
//                 int result = send(clientSocket,
//                                   reinterpret_cast<char *>(buffer.data() + bytesSent),
//                                   static_cast<int>(bytesToSend),
//                                   0);

//                 if (result == SOCKET_ERROR)
//                 {
//                     if (WSAGetLastError() == WSAEWOULDBLOCK)
//                     {
//                         Sleep(50); // Wait before retry
//                         retryCount++;
//                         continue;
//                     }
//                     std::cerr << "Error sending frame data: " << WSAGetLastError() << std::endl;
//                     return false;
//                 }

//                 bytesSent += result;
//                 retryCount = 0; // Reset retry count on successful send

//                 // Wait for acknowledgment from client with retry
//                 ackReceived = 0;
//                 while (retryCount < maxRetries)
//                 {
//                     ackReceived = recv(clientSocket, ack, sizeof(ack), 0);
//                     if (ackReceived > 0 && std::string(ack, ackReceived) == "ACK")
//                     {
//                         break;
//                     }
//                     else
//                     {
//                         retryCount++;
//                         Sleep(1000); // Wait 1 second before retrying
//                     }
//                 }

//                 if (retryCount >= maxRetries)
//                 {
//                     std::cerr << "Failed to receive acknowledgment for frame data from client" << std::endl;
//                     return false;
//                 }
//             }
//             else
//             {
//                 retryCount++;
//                 Sleep(50);
//             }
//         }

//         if (retryCount >= maxRetries)
//         {
//             std::cerr << "Max retries reached while sending frame" << std::endl;
//             return false;
//         }

//         return true;
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << "Exception in sendFrame: " << e.what() << std::endl;
//         return false;
//     }
// }

std::string ServerHandler::receiveMessage(SOCKET clientSocket)
{
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0)
    {
        return std::string(buffer, bytesReceived);
    }
    return "";
}

void ServerHandler::cleanup()
{
    if (serverSocket != INVALID_SOCKET)
    {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }
    if (initialized)
    {
        WSACleanup();
        initialized = false;
    }
}

std::vector<ServerHandler::ProcessInfo> ServerHandler::listRunningApplications()
{
    std::vector<ProcessInfo> applications;
    DWORD processes[1024], cbNeeded;

    if (EnumProcesses(processes, sizeof(processes), &cbNeeded))
    {
        int numProcesses = cbNeeded / sizeof(DWORD);
        for (int i = 0; i < numProcesses; i++)
        {
            if (processes[i] != 0)
            {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                              FALSE, processes[i]);
                if (hProcess)
                {
                    char processName[MAX_PATH] = "<unknown>";
                    HMODULE hMod;
                    PROCESS_MEMORY_COUNTERS pmc;

                    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
                    {
                        GetModuleBaseNameA(hProcess, hMod, processName, sizeof(processName));
                        ProcessInfo info;
                        info.pid = processes[i];
                        info.name = processName;

                        // Get memory info
                        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
                        {
                            info.memoryUsage = pmc.WorkingSetSize / 1024; // Convert to KB
                        }

                        applications.push_back(info);
                    }
                    CloseHandle(hProcess);
                }
            }
        }
    }

    // Sort by process name
    std::sort(applications.begin(), applications.end(),
              [](const ProcessInfo &a, const ProcessInfo &b)
              {
                  return a.name < b.name;
              });

    return applications;
}

std::vector<ServerHandler::ServiceInfo> ServerHandler::listRunningServices()
{
    std::vector<ServiceInfo> serviceList;
    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);

    if (!scManager)
    {
        std::cerr << "Failed to open service manager" << std::endl;
        return serviceList;
    }

    DWORD bytesNeeded = 0, servicesReturned = 0;
    EnumServicesStatusEx(scManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
                         NULL, 0, &bytesNeeded, &servicesReturned, NULL, NULL);

    std::vector<BYTE> buffer(bytesNeeded);
    LPENUM_SERVICE_STATUS_PROCESS services = reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESS>(buffer.data());

    if (EnumServicesStatusEx(scManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
                             buffer.data(), bytesNeeded, &bytesNeeded, &servicesReturned, NULL, NULL))
    {

        for (DWORD i = 0; i < servicesReturned; i++)
        {
            ServiceInfo info;
            info.name = services[i].lpServiceName;
            info.pid = services[i].ServiceStatusProcess.dwProcessId;

            // Get service status
            std::string status;
            switch (services[i].ServiceStatusProcess.dwCurrentState)
            {
            case SERVICE_RUNNING:
                status = "Running";
                break;
            case SERVICE_STOPPED:
                status = "Stopped";
                break;
            case SERVICE_PAUSED:
                status = "Paused";
                break;
            default:
                status = "Unknown";
            }

            serviceList.push_back(info);
        }

        std::cout << std::setfill('-') << std::setw(100) << "-" << std::endl;
    }

    CloseServiceHandle(scManager);
    return serviceList;
}

bool ServerHandler::startService(const std::string &serviceName)
{
    try
    {
        std::wstring wServiceName(serviceName.begin(), serviceName.end());
        SC_HANDLE scManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);

        if (!scManager)
        {
            std::cerr << "Failed to connect to Service Control Manager" << std::endl;
            return false;
        }

        SC_HANDLE service = OpenServiceW(scManager, wServiceName.c_str(), SERVICE_ALL_ACCESS);
        if (!service)
        {
            CloseServiceHandle(scManager);
            return false;
        }

        bool result = StartServiceW(service, 0, NULL);
        CloseServiceHandle(service);
        CloseServiceHandle(scManager);
        return result;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error starting service: " << e.what() << std::endl;
        return false;
    }
}

bool ServerHandler::openApplication(const char *appPath)
{
    // Get current directory
    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    std::cout << "Current directory: " << currentDir << std::endl;

    // Search paths
    std::vector<std::string> searchPaths = {
        std::string(currentDir) + "\\", // Current directory
        "C:\\Windows\\System32\\",      // System32
    };

    // Try each path
    for (const auto &basePath : searchPaths)
    {
        std::string fullPath = basePath + appPath;
        std::cout << "Trying path: " << fullPath << std::endl;

        if (GetFileAttributesA(fullPath.c_str()) != INVALID_FILE_ATTRIBUTES)
        {
            HINSTANCE result = ShellExecuteA(NULL, "open", fullPath.c_str(),
                                             NULL, NULL, SW_SHOWNORMAL);
            if ((intptr_t)result > 32)
            {
                std::cout << "Successfully opened: " << fullPath << std::endl;
                return true;
            }
        }

        // Try with .exe extension if no extension provided
        if (strstr(appPath, ".") == nullptr)
        {
            std::string exePath = fullPath + ".exe";
            std::cout << "Trying executable: " << exePath << std::endl;

            if (GetFileAttributesA(exePath.c_str()) != INVALID_FILE_ATTRIBUTES)
            {
                HINSTANCE result = ShellExecuteA(NULL, "open", exePath.c_str(),
                                                 NULL, NULL, SW_SHOWNORMAL);
                if ((intptr_t)result > 32)
                {
                    std::cout << "Successfully opened: " << exePath << std::endl;
                    return true;
                }
            }
        }
    }

    std::cerr << "Failed to open: " << appPath << std::endl;
    return false;
}

void ServerHandler::startWebcam()
{
    if (cap.isOpened())
    {
        std::cout << "Webcam is already running" << std::endl;
        return;
    }

    cap.open(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open webcam." << std::endl;
        return;
    }

    std::cout << "Webcam started successfully" << std::endl;
}

void ServerHandler::stopWebcam()
{
    if (!cap.isOpened())
    {
        std::cout << "Webcam is not running" << std::endl;
        return;
    }

    cap.release();
    cv::destroyAllWindows();
    std::cout << "Webcam stopped successfully" << std::endl;
}

bool ServerHandler::recordWebcam()
{
    if (!cap.isOpened())
    {
        std::cerr << "Error: Webcam is not running. Call startWebcam first." << std::endl;
        return false;
    }

    // Get executable directory
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir = std::string(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of("\\/"));

    // Set up video writer
    cv::VideoWriter videoWriter;
    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = 20;
    std::string fullPath = exeDir + "\\recorded_cam.avi";
    
    std::cout << "Recording to: " << fullPath << std::endl;
    
    videoWriter.open(fullPath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 
                    fps, cv::Size(frameWidth, frameHeight));

    if (!videoWriter.isOpened())
    {
        DWORD error = GetLastError();
        std::cerr << "Error: Could not open video writer. Error code: " << error << std::endl;
        return false;
    }

    // Record video for 3 seconds
    int frameCount = 0;
    int maxFrames = fps * 3;
    std::cout << "Recording " << maxFrames << " frames..." << std::endl;

    while (frameCount < maxFrames)
    {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Could not capture frame " << frameCount << std::endl;
            videoWriter.release();
            return false;
        }

        videoWriter.write(frame);
        frameCount++;

        cv::imshow("Server - Recording", frame);
        char key = (char)cv::waitKey(1);
        if (key == 27)
        {
            std::cout << "Recording cancelled by user" << std::endl;
            videoWriter.release();
            cv::destroyWindow("Server - Recording");
            return false;
        }
    }

    videoWriter.release();
    cv::destroyWindow("Server - Recording");
    std::cout << "Video recorded successfully to: " << fullPath << std::endl;

    return true;
}

bool ServerHandler::takeWebcamShot()
{
    cv::Mat frame;
    cap >> frame;

    if (frame.empty())
    {
        std::cerr << "Error: Could not capture frame." << std::endl;
        return false;
    }

    // Get executable directory
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir = std::string(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of("\\/"));

    std::string filename = exeDir + "\\captured_cam.jpg";
    cv::imwrite(filename, frame);
    std::cout << "Webcam photo saved as: " << filename << std::endl;
    return true;
}

void ServerHandler::listFilesInDirectory(const std::wstring &outputFile)
{
    // Get executable directory
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir = std::wstring(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/"));

    std::wstring fullPath = exeDir + L"\\" + outputFile;
    std::wofstream file(fullPath);
    if (!file)
    {
        DWORD error = GetLastError();
        std::wcerr << L"Failed to open output file: " << fullPath << std::endl;
        std::wcerr << L"Error code: " << error << std::endl;
        return;
    }

    // List files in executable directory
    for (const auto &entry : std::filesystem::directory_iterator(exeDir))
    {
        file << entry.path().wstring() << std::endl;
    }
    file.close();
}

bool ServerHandler::removeFile(const std::wstring &filename)
{
    try
    {
        if (!std::filesystem::exists(filename))
        {
            std::wcerr << L"File does not exist: " << filename << std::endl;
            return false;
        }
        return std::filesystem::remove(filename);
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "Error removing file: " << e.what() << std::endl;
        return false;
    }
}

void ServerHandler::saveApplicationsToFile(const std::vector<ProcessInfo> &applications, const std::string &filename)
{
    // Get executable directory
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir = std::string(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of("\\/"));

    // Create absolute path
    std::string fullPath = exeDir + "\\" + filename;
    std::cout << "Writing to file: " << fullPath << std::endl;

    // Get current directory for debugging
    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    std::cout << "Current directory: " << currentDir << std::endl;

    std::ofstream file(fullPath);
    if (!file.is_open())
    {
        DWORD error = GetLastError();
        std::cerr << "Failed to open file: " << fullPath << std::endl;
        std::cerr << "Error code: " << error << std::endl;
        return;
    }

    file << "List of running applications:\n";
    for (const auto &app : applications)
    {
        file << "PID: " << app.pid << ", Name: " << app.name << "\n";
    }
    file.flush();
    file.close();
}

void ServerHandler::saveServicesToFile(const std::vector<ServiceInfo> &services, const std::string &filename)
{
    // Get executable directory
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir = std::string(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of("\\/"));

    std::string fullPath = exeDir + "\\" + filename;
    std::ofstream file(fullPath);
    if (!file.is_open())
    {
        DWORD error = GetLastError();
        std::cerr << "Failed to open file: " << fullPath << std::endl;
        std::cerr << "Error code: " << error << std::endl;
        return;
    }

    file << "List of running services:\n";
    for (const auto &svc : services)
    {
        file << "PID: " << svc.pid << ", Name: " << svc.name << "\n";
    }
    file.flush();
    file.close();
}

void ServerHandler::takeScreenshot(const std::wstring &filename)
{
    // Get executable directory
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::wstring exeDir = std::wstring(exePath);
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/"));

    std::wstring fullPath = exeDir + L"\\" + filename;

    // Get the screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create compatible DC
    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP bitmap = CreateCompatibleBitmap(screenDC, screenWidth, screenHeight);
    SelectObject(memDC, bitmap);

    // Copy screen to bitmap
    BitBlt(memDC, 0, 0, screenWidth, screenHeight, screenDC, 0, 0, SRCCOPY);

    // Save bitmap to file
    Gdiplus::Bitmap bmp(bitmap, NULL);
    CLSID encoderClsid;
    GetEncoderClsid(L"image/jpeg", &encoderClsid);
    bmp.Save(fullPath.c_str(), &encoderClsid);

    std::wcout << L"Screenshot saved as: " << fullPath << std::endl;

    // Cleanup
    DeleteObject(bitmap);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);
}

bool ServerHandler::shutdownComputer()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;

    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

    if (GetLastError() != ERROR_SUCCESS)
        return false;

    return ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_APPLICATION);
}

void ServerHandler::initializeGdiplus()
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

// In server_handler.cpp, add this function:

int ServerHandler::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
    UINT num = 0;  // number of image encoders
    UINT size = 0; // size of the image encoder array in bytes

    Gdiplus::ImageCodecInfo *pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1; // Failure

    pImageCodecInfo = (Gdiplus::ImageCodecInfo *)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1; // Failure

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j; // Success
        }
    }

    free(pImageCodecInfo);
    return -1; // Failure
}

// Add to server_handler.cpp:

bool ServerHandler::terminateProcessByPID(DWORD pid)
{
    // Open process with termination rights
    HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (processHandle == NULL)
    {
        std::cerr << "Failed to open process with PID " << pid << ". Error: " << GetLastError() << std::endl;
        return false;
    }

    // Attempt to terminate process
    bool result = TerminateProcess(processHandle, 0);
    if (!result)
    {
        std::cerr << "Failed to terminate process with PID " << pid << ". Error: " << GetLastError() << std::endl;
    }
    else
    {
        std::cerr << "Process with PID " << pid << " terminated successfully." << std::endl;
    }

    CloseHandle(processHandle);
    return result;
}