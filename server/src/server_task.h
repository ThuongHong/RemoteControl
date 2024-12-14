#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

// Windows headers in correct order
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <objidl.h> // For IStream
#include <gdiplus.h>
#include <psapi.h>

// Standard headers
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <fstream>
#include <filesystem>

// OpenCV
#include <opencv2/opencv.hpp>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "ws2_32.lib")

class ServerTask
{
public:
    struct ProcessInfo
    {
        DWORD pid;
        std::string name;
        size_t memoryUsage;
    };

    struct ServiceInfo
    {
        DWORD pid;
        std::string name;
    };
    cv::VideoCapture cap;

    ServerTask();
    ~ServerTask();

    std::vector<ProcessInfo> listRunningApplications();
    void printRunningApplications(const std::vector<ProcessInfo> &applications);
    void saveApplicationsToFile(const std::vector<ProcessInfo> &applications, const std::string &filename);

    std::vector<ServiceInfo> listRunningServices();
    void printRunningServices(const std::vector<ServiceInfo> &services);
    void saveServicesToFile(const std::vector<ServiceInfo> &services, const std::string &filename);

    void takeScreenshot(const std::wstring &filename);
    bool terminateProcessByPID(DWORD pid);
    bool startService(const std::string &serviceName); // Start service
    bool openApplication(const char *appPath);         // Start application
    bool shutdownComputer();
    void startWebcam();
    void stopWebcam();
    void listFilesInDirectory(const std::wstring &outputFile);
    bool removeFile(const std::wstring &filename);

private:
    ULONG_PTR gdiplusToken;

    void webcamThread();
    std::atomic<bool> stopWebcamFlag;
    std::thread webcam;

    void initializeGdiplus();
    int GetEncoderClsid(const WCHAR *format, CLSID *pClsid);
    void saveBitmapToJpeg(HBITMAP hBitmap, const WCHAR *filename);
};

#endif // SERVER_H
