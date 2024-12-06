#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <string>
#include <windows.h>
#include <psapi.h>
#include <gdiplus.h>
#include <opencv2/opencv.hpp>
#include <atomic>
#include <thread>
#include <filesystem>

class ServerTask
{
public:
    struct ProcessInfo
    {
        DWORD pid;
        std::string name;
    };

    struct ServiceInfo
    {
        DWORD pid;
        std::string name;
    };

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
    bool startService(const char *serviceName);
    bool openApplication(const char *appPath);
    bool shutdownComputer();
    void startWebcam();
    void stopWebcam();
    void listFilesInDirectory(const std::string &outputFile);

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
