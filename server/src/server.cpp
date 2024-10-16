// server.cpp : Defines the entry point for the application.
//

#include "server_socket.h"
#include "server.h"


struct ProcessInfo {
    DWORD pid;
    std::string name;
};

struct ServiceInfo {
    DWORD pid;
    std::string name;
};

// Function to list running applications with PID and count
std::vector<ProcessInfo> listRunningApplications() {
    DWORD processes[1024], count, processID;
    unsigned int totalApplications = 0;
    std::vector<ProcessInfo> applicationList;

    // Retrieve the list of process identifiers
    if (!EnumProcesses(processes, sizeof(processes), &count)) {
        std::cerr << "Failed to enumerate processes." << std::endl;
        return applicationList;
    }

    // Calculate the number of process identifiers
    count /= sizeof(DWORD);

    // Loop over each process
    for (unsigned int i = 0; i < count; i++) {
        processID = processes[i];
        if (processID != 0) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
            if (hProcess) {
                char processName[MAX_PATH] = "<unknown>";

                // Get the process name
                HMODULE hMod;
                DWORD cbNeeded;
                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                    GetModuleBaseName(hProcess, hMod, processName, sizeof(processName) / sizeof(char));
                }

                // Add to list if it's not a system process
                if (strcmp(processName, "System") != 0 && strcmp(processName, "svchost.exe") != 0) {
                    ProcessInfo info = { processID, processName };
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

void printRunningApplications(const std::vector<ProcessInfo>& applications) {
    for (const auto& app : applications) {
        std::cout << "Application: " << app.name << ", PID: " << app.pid << std::endl;
    }
}

// Function to save running applications to a file
void saveApplicationsToFile(const std::vector<ProcessInfo>& applications, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "List of running applications:\n";
    for (const auto& app : applications) {
        file << "Application: " << app.name << ", PID: " << app.pid << "\n";
    }

    file << "Total applications: " << applications.size() << "\n";
    file.close();
}



// Function to list running services with PID and count
std::vector<ServiceInfo> listRunningServices() {
    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (scManager == NULL) {
        std::cerr << "Failed to open service manager." << std::endl;
        return {};
    }

    DWORD bytesNeeded = 0, serviceCount = 0;
    EnumServicesStatusEx(scManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &bytesNeeded, &serviceCount, NULL, NULL);

    std::vector<BYTE> buffer(bytesNeeded);
    LPENUM_SERVICE_STATUS_PROCESS services = reinterpret_cast<LPENUM_SERVICE_STATUS_PROCESS>(buffer.data());

    if (!EnumServicesStatusEx(scManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, reinterpret_cast<LPBYTE>(services), bytesNeeded, &bytesNeeded, &serviceCount, NULL, NULL)) {
        std::cerr << "Failed to enumerate services." << std::endl;
        CloseServiceHandle(scManager);
        return {};
    }

    std::vector<ServiceInfo> serviceList;
    for (DWORD i = 0; i < serviceCount; i++) {
        ServiceInfo info;
        info.name = services[i].lpServiceName;
        info.pid = services[i].ServiceStatusProcess.dwProcessId;
        serviceList.push_back(info);
    }

    std::cout << "Total running services: " << serviceCount << std::endl;

    CloseServiceHandle(scManager);
    return serviceList;
}

void printRunningServices(const std::vector<ServiceInfo>& services) {
    for (const auto& svc : services) {
        std::cout << "Service: " << svc.name << ", PID: " << svc.pid << std::endl;
    }
}

// Function to save running services to a file
void saveServicesToFile(const std::vector<ServiceInfo>& services, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "List of running services:\n";
    for (const auto& svc : services) {
        file << "Service: " << svc.name << ", PID: " << svc.pid << "\n";
    }

    file << "Total services: " << services.size() << "\n";
    file.close();
}

// Function to initialize GDI+
void initializeGdiplus(ULONG_PTR& gdiplusToken) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

// Function to get the CLSID of the JPEG encoder
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;          // Number of image encoders
    UINT size = 0;         // Size of the image encoder array in bytes

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) {
        return -1;  // Failure to find encoder
    }

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) {
        return -1;  // Failure to allocate memory
    }

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Return index of found encoder
        }
    }

    free(pImageCodecInfo);
    return -1;  // Not found
}

// Function to save HBITMAP to JPEG file
void SaveBitmapToJpeg(HBITMAP hBitmap, const WCHAR* filename) {
    Gdiplus::Bitmap bitmap(hBitmap, NULL);

    CLSID clsid;
    // Get CLSID of JPEG encoder
    if (GetEncoderClsid(L"image/jpeg", &clsid) != -1) {
        // Save image to file
        bitmap.Save(filename, &clsid, NULL);
    }
    else {
        std::wcout << L"Could not find JPEG encoder." << std::endl;
    }
}

// TakeScreenshot function to save screenshot with specified filename
void TakeScreenshot(const std::wstring& filename) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Get the screen dimensions
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);

    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

    BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hMemoryDC, hOldBitmap);

    SaveBitmapToJpeg(hBitmap, filename.c_str());  // Save using the specified filename

    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);
    DeleteObject(hBitmap);

    Gdiplus::GdiplusShutdown(gdiplusToken);
}

int main() {
    //std::wstring filename = L"3.jpg";
    //TakeScreenshot(filename);
    //std::wcerr << "Screenshot saved as " << filename << std::endl;

    if (!initializeWinsock()) {
        return 1;
    }

    SOCKET serverSocket = createServerSocket();
    if (serverSocket == INVALID_SOCKET) {
        WSACleanup();
        return 1;
    }

    if (bindServerSocket(serverSocket, 8080) && listenForConnections(serverSocket)) {
        std::cout << "Server is listening on port 8080..." << std::endl;

        SOCKET clientSocket = acceptClient(serverSocket);
        if (clientSocket != INVALID_SOCKET) {
            std::cout << "Client connected!" << std::endl;
            //if (sendFile(clientSocket, filename)) {  // Replace with your actual file path
            //    std::cout << "File sent to client!" << std::endl;
            //}
            //closesocket(clientSocket);
            while (true) {
                std::string receivedMessage;
                if (receiveString(clientSocket, receivedMessage)) {
                    std::cout << "Received message: " << receivedMessage << std::endl;
                    if (receivedMessage.substr(0, 8) == "list app") {
                        std::cout << "Listing running applications:\n";
                        std::vector<ProcessInfo> applications = listRunningApplications();
                        saveApplicationsToFile(applications, "applications_list.txt");
                        if (sendFile(clientSocket, L"applications_list.txt"))
                            std::cout << "File sent to client!" << std::endl;
                    }
                    else if (receivedMessage.substr(0, 12) == "list service") {
                        std::cout << "\nListing running services:\n";
                        std::vector<ServiceInfo> services = listRunningServices();
                        saveServicesToFile(services, "services_list.txt");
                        if (sendFile(clientSocket, L"services_list.txt"))
                            std::cout << "File sent to client!" << std::endl;
                    }
                    else if (receivedMessage.substr(0, 15) == "take screenshot") {
                        std::wstring filename = L"screenshot.jpg";
                        TakeScreenshot(filename);
                        std::wcerr << "Screenshot saved as " << filename << std::endl;
                        if (sendFile(clientSocket, filename)) {  // Replace with your actual file path
                            std::cout << "File sent to client!" << std::endl;
                        }
                    }
                    else if (receivedMessage == "end") break;
                }
            }

        }
    }

    cleanup(serverSocket);


    //std::cout << "Listing running applications:\n";
    //std::vector<ProcessInfo> applications = listRunningApplications();
    //saveApplicationsToFile(applications, "applications_list.txt");


    //std::cout << "\nListing running services:\n";
    //std::vector<ServiceInfo> services = listRunningServices();
    //printRunningServices(services);

    return 0;
}
