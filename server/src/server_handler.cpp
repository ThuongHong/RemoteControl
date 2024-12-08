// server_handler.cpp

#include "server_handler.h"

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
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Cannot open file" << std::endl;
        return false;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Send file size
    send(clientSocket, (char *)&fileSize, sizeof(fileSize), 0);

    // Send file content
    char buffer[1024];
    while (!file.eof())
    {
        file.read(buffer, sizeof(buffer));
        send(clientSocket, buffer, file.gcount(), 0);
    }

    file.close();
    return true;
}

bool ServerHandler::sendFrame(SOCKET clientSocket, cv::Mat &frame)
{
    try
    {
        if (frame.empty() || clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Invalid frame or socket" << std::endl;
            return false;
        }

        // Encode frame
        std::vector<uchar> buffer;
        if (!cv::imencode(".jpg", frame, buffer, {cv::IMWRITE_JPEG_QUALITY, 80}))
        {
            std::cerr << "Failed to encode frame" << std::endl;
            return false;
        }

        // Send frame size
        int frameSize = static_cast<int>(buffer.size());
        int frameSizeNetworkOrder = htonl(frameSize);

        // Wait for socket to be ready
        fd_set writeSet;
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&writeSet);
        FD_SET(clientSocket, &writeSet);

        if (select(0, NULL, &writeSet, NULL, &timeout) > 0)
        {
            if (send(clientSocket, reinterpret_cast<char *>(&frameSizeNetworkOrder),
                     sizeof(frameSizeNetworkOrder), 0) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    std::cerr << "Error sending frame size: " << WSAGetLastError() << std::endl;
                    return false;
                }
            }
        }

        // Send frame data in chunks with retry
        const size_t chunkSize = MAX_PACKET_SIZE;
        size_t bytesSent = 0;
        int retryCount = 0;
        const int maxRetries = 5;

        while (bytesSent < buffer.size() && retryCount < maxRetries)
        {
            size_t remainingBytes = buffer.size() - bytesSent;
            size_t bytesToSend = std::min(remainingBytes, chunkSize);

            // Wait for socket to be ready
            FD_ZERO(&writeSet);
            FD_SET(clientSocket, &writeSet);

            if (select(0, NULL, &writeSet, NULL, &timeout) > 0)
            {
                int result = send(clientSocket,
                                  reinterpret_cast<char *>(buffer.data() + bytesSent),
                                  static_cast<int>(bytesToSend),
                                  0);

                if (result == SOCKET_ERROR)
                {
                    if (WSAGetLastError() == WSAEWOULDBLOCK)
                    {
                        Sleep(50); // Wait before retry
                        retryCount++;
                        continue;
                    }
                    std::cerr << "Error sending frame data: " << WSAGetLastError() << std::endl;
                    return false;
                }

                bytesSent += result;
                retryCount = 0; // Reset retry count on successful send
            }
            else
            {
                retryCount++;
                Sleep(50);
            }
        }

        if (retryCount >= maxRetries)
        {
            std::cerr << "Max retries reached while sending frame" << std::endl;
            return false;
        }

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in sendFrame: " << e.what() << std::endl;
        return false;
    }
}

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

// Process Methods
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
                    if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
                    {
                        GetModuleBaseNameA(hProcess, hMod, processName, sizeof(processName));
                        ProcessInfo info = {processes[i], processName};
                        applications.push_back(info);
                    }
                    CloseHandle(hProcess);
                }
            }
        }
    }
    return applications;
}

// Service Methods
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
            serviceList.push_back(info);
        }
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
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(appPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        std::cerr << "Failed to open application: " << appPath << std::endl;
        return false;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

void ServerHandler::startWebcam(SOCKET clientSocket)
{
    cap.open(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open webcam." << std::endl;
        return;
    }

    // cv::namedWindow("Server - Webcam", cv::WINDOW_AUTOSIZE);

    // Set socket to non-blocking mode
    u_long mode = 1;
    ioctlsocket(clientSocket, FIONBIO, &mode);

    while (true)
    {
        char buffer[1024];
        int result = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (result > 0)
        {
            std::string message(buffer, result);
            if (message == "stop")
            {
                std::cout << "Received stop message from client." << std::endl;
                break;
            }
        }
        // Capture and process frame
        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Could not capture frame." << std::endl;
            break;
        }

        // Send frame to client
        if (!sendFrame(clientSocket, frame))
        {
            std::cerr << "Failed to send frame to client." << std::endl;
            break;
        }

        // cv::imshow("Server - Webcam", frame);
    }

    // Reset socket to blocking mode
    mode = 0;
    ioctlsocket(clientSocket, FIONBIO, &mode);

    // Cleanup
    cap.release();
    // cv::destroyWindow("Server - Webcam");
    std::cout << "Webcam stopped" << std::endl;
}

void ServerHandler::listFilesInDirectory(const std::wstring &outputFile)
{
    std::wofstream file(outputFile);
    if (!file)
    {
        std::wcerr << L"Failed to open output file" << std::endl;
        return;
    }

    for (const auto &entry : std::filesystem::directory_iterator("."))
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

// server_handler.cpp
void ServerHandler::saveApplicationsToFile(const std::vector<ProcessInfo> &applications, const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "List of running applications:\n";
    for (const auto &app : applications)
    {
        file << "PID: " << app.pid << ", Name: " << app.name << "\n";
    }
    file.close();
}

void ServerHandler::saveServicesToFile(const std::vector<ServiceInfo> &services, const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "List of running services:\n";
    for (const auto &svc : services)
    {
        file << "PID: " << svc.pid << ", Name: " << svc.name << "\n";
    }
    file.close();
}

void ServerHandler::takeScreenshot(const std::wstring &filename)
{
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
    bmp.Save(filename.c_str(), &encoderClsid);

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