#pragma once

#include <string>
#include <functional>
#include <thread>
#include <atomic>

class HokuyoTcpReceiver {
public:
    using DataCallback = std::function<void(const std::string&)>;

    HokuyoTcpReceiver(const std::string& ip, int port = 10940);
    ~HokuyoTcpReceiver();

    bool connectToLidar();
    void start();
    void stop();

    void setCallback(DataCallback cb);

private:
    void receiveLoop();
    bool sendCommand(const std::string& cmd);

private:
    std::string lidarIp;
    int lidarPort;

    int sockfd;
    std::thread recvThread;
    std::atomic<bool> running;

    DataCallback callback;
};
