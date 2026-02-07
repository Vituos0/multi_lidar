// #include <iostream>
// #include "lidar/UdpReceiver.h"

// int main()
// {
//     UdpReceiver lidar1(2368); // ví dụ port LIDAR

//     lidar1.setCallback([](const std::vector<uint8_t>& data)
//     {
//         std::cout << "Received packet: " << data.size() << " bytes\n";
//     });

//     lidar1.start();

//     std::cout << "Listening UDP...\n";
//     std::cin.get();

//     lidar1.stop();
//     return 0;
// }  // One thread


// #include "Lidar/HokuyoTcpReceiver.h"
// #include <iostream>

// int main() {
//     HokuyoTcpReceiver hokuyo("192.168.0.30");

//     if (!hokuyo.connectToLidar())
//         return -1;

//     hokuyo.setCallback([](const std::string& data) {
//         std::cout << "Hokuyo raw:\n" << data << "\n";
//     });

//     hokuyo.start();
//     std::cin.get();
//     hokuyo.stop();
// }
// #include "lidar/UdpReceiver.h"
// #include <iostream>
// #include "lidar/PavoParser.h"

// int main() {
//     UdpReceiver receiver(2368); // port LS20H

//     receiver.setCallback([](const std::string& ip,
//                             const uint8_t* data,
//                             size_t len)
//     {
//         if (ip == "10.10.10.101") {
//             std::cout << "[LiDAR 1] " << len << " bytes\n";
//         }
//         else if (ip == "10.10.10.104") {
//             std::cout << "[LiDAR 2 ] " << len << " bytes\n";
//         }
//         else {
//             std::cout << "[UNKNOWN " << ip << "]\n";
//         }
//     });

//     if (!receiver.start()) {
//         std::cerr << "Failed to start UDP receiver\n";
//         return -1;
//     }

//     std::cout << "Receiving LiDAR packets...\n";
//     std::cin.get();

//     receiver.stop();
//     return 0;

// } debug log packet

#include <iostream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include "lidar/PavoParser.h"

constexpr int UDP_PORT = 2368;
constexpr int BUFFER_SIZE = 2048;

// IP cố định của từng LiDAR
constexpr const char* LIDAR1_IP = "10.10.10.101";
constexpr const char* LIDAR2_IP = "10.10.10.102";

int getLidarIdFromIP(const sockaddr_in& addr)
{
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ipStr, sizeof(ipStr));

    if (strcmp(ipStr, LIDAR1_IP) == 0) return 1;
    if (strcmp(ipStr, LIDAR2_IP) == 0) return 2;

    return -1; // unknown
}

void onUdpPacket(const uint8_t* data, size_t len,
                 int lidarId,
                 const std::string& ip)
{
    std::vector<LidarPoint> points;

    if (!PavoParser::parse(data, len, points))
    {
        std::cerr << "[DROP] LiDAR " << lidarId
                  << " IP=" << ip
                  << " len=" << len << std::endl;
        return;
    }

    for (auto& p : points)
    {
        std::cout << "[LiDAR " << lidarId << "] "
                  << "A=" << p.angle
                  << " D=" << p.distance
                  << " I=" << (int)p.intensity
                  << std::endl;
    }
}


int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(UDP_PORT);
    localAddr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (sockaddr*)&localAddr, sizeof(localAddr));

    std::cout << "Listening UDP port " << UDP_PORT << "...\n";

    uint8_t buffer[BUFFER_SIZE];

    while (true)
    {
        sockaddr_in senderAddr{};
        int senderLen = sizeof(senderAddr);

        int recvLen = recvfrom(
            sock,
            reinterpret_cast<char*>(buffer),
            BUFFER_SIZE,
            0,
            (sockaddr*)&senderAddr,
            &senderLen
        );

        if (recvLen <= 0)
            continue;

        int lidarId = getLidarIdFromIP(senderAddr);
        if (lidarId < 0)
            continue; // bỏ gói lạ

        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &senderAddr.sin_addr, ipStr, sizeof(ipStr));

        onUdpPacket(buffer, recvLen, lidarId, ipStr);

    }

    closesocket(sock);
    WSACleanup();
    return 0;
}




