// #include "UdpReceiver.h"
// #include <winsock2.h>
// #include <ws2tcpip.h>
// #include <iostream>

// #pragma comment(lib, "ws2_32.lib")

// UdpReceiver::UdpReceiver(int port)
//     : m_port(port)
// {
//     WSADATA wsa;
//     WSAStartup(MAKEWORD(2, 2), &wsa);
// }

// UdpReceiver::~UdpReceiver()
// {
//     stop();
//     WSACleanup();
// }

// void UdpReceiver::setCallback(PacketCallback cb)
// {
//     m_callback = cb;
// }

// void UdpReceiver::start()
// {
//     m_running = true;
//     m_thread = std::thread(&UdpReceiver::receiveLoop, this);
// }

// void UdpReceiver::stop()
// {
//     m_running = false;
//     if (m_thread.joinable())
//         m_thread.join();
// }

// void UdpReceiver::receiveLoop()
// {
//     SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

//     sockaddr_in addr{};
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(m_port);
//     addr.sin_addr.s_addr = INADDR_ANY;

//     bind(sock, (sockaddr*)&addr, sizeof(addr));

//     while (m_running)
//     {
//         uint8_t buffer[2048];
//         sockaddr_in from{};
//         int fromLen = sizeof(from);

//         int bytes = recvfrom(
//             sock,
//             (char*)buffer,
//             sizeof(buffer),
//             0,
//             (sockaddr*)&from,
//             &fromLen
//         );

//         if (bytes > 0 && m_callback)
//         {
//             m_callback({ buffer, buffer + bytes });
//         }
//     }

//     closesocket(sock);
// } one thread process

#include "UdpReceiver.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

UdpReceiver::UdpReceiver(int listenPort)
    : port(listenPort), sockfd(-1), running(false) {}

UdpReceiver::~UdpReceiver() {
    stop();
}

bool UdpReceiver::start() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed\n";
        return false;
    }

    running = true;
    recvThread = std::thread(&UdpReceiver::receiveLoop, this);
    return true;
}

void UdpReceiver::stop() {
    running = false;
    if (recvThread.joinable()) recvThread.join();
    if (sockfd >= 0) closesocket(sockfd);
    WSACleanup();
}

void UdpReceiver::setCallback(PacketCallback cb) {
    callback = cb;
}

void UdpReceiver::receiveLoop() {
    uint8_t buffer[2048];

    while (running) {
        sockaddr_in src{};
        int srcLen = sizeof(src);

        int len = recvfrom(
            
            sockfd,
            reinterpret_cast<char*>(buffer),
            sizeof(buffer),
            0,
            (sockaddr*)&src,
            &srcLen
        );
        //-------------
         printf("RAW PACKET (%d bytes):\n", len);
    for (int i = 0; i < len; ++i) {
        printf("%02X ", buffer[i]);
    }
    printf("\n\n");
        //----------------

        if (len > 0 && callback) {
            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &src.sin_addr, ipStr, sizeof(ipStr));

            callback(std::string(ipStr), buffer, len);
        }
    }
}

