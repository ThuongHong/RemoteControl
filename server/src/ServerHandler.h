#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

// Windows headers in correct order
#include <winsock2.h>
#include <windows.h>
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

// Constants for buffer management
const size_t MAX_PACKET_SIZE = 1400;            // Maximum packet size
const size_t MAX_BUFFER_SIZE = 8192;            // 8KB chunks
const size_t MAX_FILE_SIZE = 100 * 1024 * 1024; // 100MB limit

class ServerHandler
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
        size_t memoryUsage;
    };

    ServerHandler();
    ~ServerHandler();

    // Socket methods
    bool initialize();
    bool bindAndListen(int port);
    SOCKET acceptClient();
    bool sendMessage(SOCKET clientSocket, const std::string &message);
    bool sendFile(SOCKET clientSocket, const std::wstring &filename);
    // void sendFrame(SOCKET clientSocket, const cv::Mat &frame);
    std::string receiveMessage(SOCKET clientSocket);
    void cleanup();

    // Task methods
    std::vector<ProcessInfo> listRunningApplications();
    std::vector<ServiceInfo> listRunningServices();
    void saveApplicationsToFile(const std::vector<ProcessInfo> &applications, const std::string &filename);
    void saveServicesToFile(const std::vector<ServiceInfo> &services, const std::string &filename);
    void takeScreenshot(const std::wstring &filename);
    bool terminateProcessByPID(DWORD pid);
    bool startService(const std::string &serviceName);
    bool openApplication(const char *appPath);
    bool shutdownComputer();
    void startWebcam();
    void stopWebcam();
    bool recordWebcam();
    bool takeWebcamShot();
    void listFilesInDirectory(const std::wstring &outputFile);
    bool removeFile(const std::wstring &filename);

private:
    // Socket members
    SOCKET serverSocket;
    WSADATA wsaData;
    bool initialized;

    // Task members
    cv::VideoCapture cap;
    std::thread webcam;
    std::atomic<bool> stopWebcamFlag;
    SOCKET clientSock, serverSock;

    // Helper methods
    void initializeGdiplus();
    int GetEncoderClsid(const WCHAR *format, CLSID *pClsid);
    void saveBitmapToJpeg(HBITMAP hBitmap, const WCHAR *filename);
    // bool sendFrame(SOCKET clientSocket, cv::Mat &frame);
    bool isClientConnected;
};

#endif // SERVER_HANDLER_H