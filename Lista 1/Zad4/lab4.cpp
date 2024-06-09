#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

int main() {
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        std::cerr << "Can't initialize Winsock! Quitting" << std::endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Can't create socket! Quitting" << std::endl;
        return 1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR) {
        std::cerr << "Can't connect to server! Quitting" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    int n, p;
    std::cin >> n >> p;

    send(sock, (char*)&n, sizeof(int), 0);
    send(sock, (char*)&p, sizeof(int), 0);
    double result;
    int bytesReceived = recv(sock, (char*)&result, sizeof(double), 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Error in receiving response from server! Quitting" << std::endl;
    } else {
        std::cout << "Partial sum received from server: " << result << std::endl;
    }

    // Close socket
    closesocket(sock);
    WSACleanup();

    return 0;
}