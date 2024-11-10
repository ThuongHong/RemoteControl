#include "server_socket.h"
#include "server_task.h"

ServerTask::ServerTask() : stopWebcamFlag(false)
{
    initializeGdiplus();
}

ServerTask::~ServerTask()
{
    stopWebcam();
    Gdiplus::GdiplusShutdown(gdiplusToken);
}

void ServerTask::initializeGdiplus()
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

std::vector<ServerTask::ProcessInfo> ServerTask::listRunningApplications()
{
    DWORD processes[1024], count, processID;
    unsigned int totalApplications = 0;
    std::vector<ProcessInfo> applicationList;

    if (!EnumProcesses(processes, sizeof(processes), &count))
    {
        std::cerr << "Failed to enumerate processes." << std::endl;
        return applicationList;
    }

    count /= sizeof(DWORD);

    for (unsigned int i = 0; i < count; i++)
    {
        processID = processes[i];
        if (processID != 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
            if (hProcess)
            {
                char processName[MAX_PATH] = "<unknown>";
                HMODULE hMod;
                DWORD cbNeeded;
                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
                {
                    GetModuleBaseName(hProcess, hMod, processName, sizeof(processName) / sizeof(char));
                }

                if (strcmp(processName, "System") != 0 && strcmp(processName, "svchost.exe") != 0)
                {
                    ProcessInfo info = {processID, processName};
                    applicationList.push_back(info);
                    totalApplications++;
                }

                CloseHandle(hProcess);
            }
        }
    }

    std::cout << "Total running applications: " << totalApplications << std::endl;
    return applicationList;
}

void ServerTask::printRunningApplications(const std::vector<ProcessInfo> &applications)
{
    for (const auto &app : applications)
    {
        std::cout << "Application: " << app.name << ", PID: " << app.pid << std::endl;
    }
}

void ServerTask::saveApplicationsToFile(const std::vector<ProcessInfo> &applications, const std::string &filename)
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
        file << "Application: " << app.name << ", PID: " << app.pid << "\n";
    }

    file << "Total applications: " << applications.size() << "\n";
    file.close();
}

std::vector<ServerTask::ServiceInfo> ServerTask::listRunningServices()
{
    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (scManager == NULL)
    {
        std::cerr << "Failed to open service manager." << std::endl;
        return {};
    }

    DWORD bytesNeeded = 0, serviceCount = 0;
    EnumServicesStatusEx(scManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &bytesNeeded, &serviceCount, NULL, NULL);

    std::vector<BYTE> buffer(bytesNeeded);
    LPENUM_SERVICE_STATUS_PROCESS services = reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESS>(buffer.data());

    if (!EnumServicesStatusEx(scManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, reinterpret_cast<LPBYTE>(services), bytesNeeded, &bytesNeeded, &serviceCount, NULL, NULL))
    {
        std::cerr << "Failed to enumerate services." << std::endl;
        CloseServiceHandle(scManager);
        return {};
    }

    std::vector<ServiceInfo> serviceList;
    for (DWORD i = 0; i < serviceCount; i++)
    {
        ServiceInfo info;
        info.name = services[i].lpServiceName;
        info.pid = services[i].ServiceStatusProcess.dwProcessId;
        serviceList.push_back(info);
    }

    std::cout << "Total running services: " << serviceCount << std::endl;

    CloseServiceHandle(scManager);
    return serviceList;
}

void ServerTask::printRunningServices(const std::vector<ServiceInfo> &services)
{
    for (const auto &svc : services)
    {
        std::cout << "Service: " << svc.name << ", PID: " << svc.pid << std::endl;
    }
}

void ServerTask::saveServicesToFile(const std::vector<ServiceInfo> &services, const std::string &filename)
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
        file << "Service: " << svc.name << ", PID: " << svc.pid << "\n";
    }

    file << "Total services: " << services.size() << "\n";
    file.close();
}

int ServerTask::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
    UINT num = 0;
    UINT size = 0;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
    {
        return -1;
    }

    Gdiplus::ImageCodecInfo *pImageCodecInfo = (Gdiplus::ImageCodecInfo *)(malloc(size));
    if (pImageCodecInfo == NULL)
    {
        return -1;
    }

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

void ServerTask::saveBitmapToJpeg(HBITMAP hBitmap, const WCHAR *filename)
{
    Gdiplus::Bitmap bitmap(hBitmap, NULL);
    CLSID clsid;

    if (GetEncoderClsid(L"image/jpeg", &clsid) != -1)
    {
        bitmap.Save(filename, &clsid, NULL);
    }
    else
    {
        std::wcout << L"Could not find JPEG encoder." << std::endl;
    }
}

void ServerTask::takeScreenshot(const std::wstring &filename)
{
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);

    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

    BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hMemoryDC, hOldBitmap);

    saveBitmapToJpeg(hBitmap, filename.c_str());

    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);
    DeleteObject(hBitmap);
}

bool ServerTask::terminateProcessByPID(DWORD pid)
{
    HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (processHandle == NULL)
    {
        std::cerr << "Failed to open process with PID " << pid << ". Error: " << GetLastError() << std::endl;
        return false;
    }

    bool result = TerminateProcess(processHandle, 0);
    if (!result)
    {
        std::cerr << "Failed to terminate process with PID " << pid << ". Error: " << GetLastError() << std::endl;
    }
    else
    {
        std::cout << "Process with PID " << pid << " terminated successfully." << std::endl;
    }

    CloseHandle(processHandle);
    return result;
}

bool ServerTask::startService(const char *serviceName)
{
    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (scManager == NULL)
    {
        std::cerr << "Failed to connect to service control manager. Error: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE service = OpenService(scManager, serviceName, SERVICE_START);
    if (service == NULL)
    {
        std::cerr << "Failed to open service. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(scManager);
        return false;
    }

    if (!StartService(service, 0, NULL))
    {
        std::cerr << "Failed to start service. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(service);
        CloseServiceHandle(scManager);
        return false;
    }

    std::cout << "Service " << serviceName << " started successfully." << std::endl;

    CloseServiceHandle(service);
    CloseServiceHandle(scManager);
    return true;
}

bool ServerTask::openApplication(const char *appPath)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(appPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        std::cerr << "Failed to open application: " << appPath << ". Error: " << GetLastError() << std::endl;
        return false;
    }

    std::cout << "Application " << appPath << " opened successfully." << std::endl;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

bool ServerTask::shutdownComputer()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tokenPriv;

    // Open a handle to the process token
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        std::cerr << "Failed to open process token. Error: " << GetLastError() << std::endl;
        return false;
    }

    // Get the LUID for shutdown privilege
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tokenPriv.Privileges[0].Luid);
    tokenPriv.PrivilegeCount = 1;
    tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Adjust token privileges
    AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, 0, NULL, 0);
    if (GetLastError() != ERROR_SUCCESS)
    {
        std::cerr << "Failed to adjust token privileges. Error: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return false;
    }

    // Initiate shutdown
    bool result = ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
    if (!result)
    {
        std::cerr << "Failed to shut down the computer. Error: " << GetLastError() << std::endl;
    }

    CloseHandle(hToken);
    return result;
}
void ServerTask::webcamThread()
{
    cv::VideoCapture cap(0); // Open default camera (0)

    if (!cap.isOpened())
    {
        std::cerr << "Error: Cannot open camera!" << std::endl;
        return;
    }

    cv::Mat frame;
    while (!stopWebcamFlag)
    {                 // Check flag to stop the webcam
        cap >> frame; // Capture frame from the webcam

        if (frame.empty())
        {
            std::cerr << "Error: Cannot retrieve frame!" << std::endl;
            break;
        }

        cv::imshow("Webcam", frame); // Display the frame

        // Listen for ESC to exit
        if (cv::waitKey(30) == 27)
        { // Press ESC to exit (turn off webcam)
            stopWebcamFlag = true;
            break;
        }
    }

    cap.release();           // Release the camera
    cv::destroyAllWindows(); // Close all OpenCV windows
}

void ServerTask::startWebcam()
{
    if (!webcam.joinable())
    {                                                          // Check if the webcam thread is not running
        stopWebcamFlag = false;                                // Set flag to allow webcam to run
        webcam = std::thread(&ServerTask::webcamThread, this); // Create a new thread for the webcam
        std::cout << "Webcam is turned on..." << std::endl;
    }
    else
    {
        std::cout << "Webcam is already running!" << std::endl;
    }
}

void ServerTask::stopWebcam()
{
    if (webcam.joinable())
    {
        stopWebcamFlag = true;   // Set flag to stop the webcam thread
        webcam.join();           // Wait for the thread to finish
        cv::destroyAllWindows(); // Ensure all OpenCV windows are closed
        std::cout << "Webcam is turned off." << std::endl;
    }
    else
    {
        std::cout << "Webcam is not turned on!" << std::endl;
    }
}
