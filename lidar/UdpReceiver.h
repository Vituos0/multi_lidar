// #pragma once
// #include<thread>
// #include<atomic>
// #include<functional>
// #include <vector>

// class UdpReceiver
// {
// public:
//     using PacketCallback = std::function<void(const std::vector<uint8_t>&)>;

//     UdpReceiver(int port);
//     ~UdpReceiver();

//     void start();
//     void stop();

//     void setCallback(PacketCallback packageCb);
// private:
//     void receiveLoop();

//     int m_port;
//     std::thread m_thread;
//     std::atomic<bool> m_running{false};
//     PacketCallback m_callback;


// }; v1: UDP 1 thread

#pragma once
#include <string>
#include <functional>
#include <atomic>
#include <thread>

class UdpReceiver {
public:
    using PacketCallback = std::function<void(
        const std::string& srcIp,
        const uint8_t* data,
        size_t length
    )>;

    UdpReceiver(int listenPort);
    ~UdpReceiver();

    bool start();
    void stop();

    void setCallback(PacketCallback cb);

private:
    void receiveLoop();

    int port;
    int sockfd;
    std::atomic<bool> running;
    std::thread recvThread;

    PacketCallback callback;
};
