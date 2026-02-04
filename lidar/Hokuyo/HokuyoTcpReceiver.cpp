#include "HokuyoTcpReceiver.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

HokuyoTcpReceiver::HokuyoTcpReceiver(const std::string& ip, int port)
    : lidarIp(ip), lidarPort(port), sockfd(-1), running(false) {
#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

HokuyoTcpReceiver::~HokuyoTcpReceiver() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool HokuyoTcpReceiver::connectToLidar() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(lidarPort);
    addr.sin_addr.s_addr = inet_addr(lidarIp.c_str());

    if (connect(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Connect failed\n";
        return false;
    }

    std::cout << "Connected to Hokuyo " << lidarIp << "\n";
    return true;
}

void HokuyoTcpReceiver::start() {
    if (running) return;
    running = true;

    // SCIP 2.0: Start continuous scan (MD command)
    sendCommand("MD0044072501100\n");

    recvThread = std::thread(&HokuyoTcpReceiver::receiveLoop, this);
}

void HokuyoTcpReceiver::stop() {
    running = false;
    if (recvThread.joinable())
        recvThread.join();

#ifdef _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif
}

void HokuyoTcpReceiver::setCallback(DataCallback cb) {
    callback = cb;
}

bool HokuyoTcpReceiver::sendCommand(const std::string& cmd) {
    int sent = send(sockfd, cmd.c_str(), (int)cmd.size(), 0);
    return sent == (int)cmd.size();
}

void HokuyoTcpReceiver::receiveLoop() {
    char buffer[4096];

    while (running) {
        int len = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0) break;

        buffer[len] = '\0';
        std::string data(buffer);

        if (callback) {
            callback(data);
        }
    }
}
