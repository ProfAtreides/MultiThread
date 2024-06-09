#include <iostream>
#include <string>
#include <vector>
#include <WS2tcpip.h>
#include <thread>
#include <cmath>
#include <mutex>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

double calcPartialSum(int start, int end) {
    double partial_sum = 0;
    for (int i = start; i <= end; i++) {
        partial_sum += 1.0 / i;
    }
    return partial_sum;
}

void calcPartialSumLink(int start, int end, double& result) {
    result = calcPartialSum(start, end);
}

double calcGamma(int n, int p) {
    vector<thread> threads;
    vector<double> partialSums(p);

    for (int i = 0; i < p; i++) {
        int start = 1 + (n / p) * i;
        int end = std::min(n, (n / p) * (i + 1));
        threads.emplace_back(calcPartialSumLink, start, end, ref(partialSums[i]));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    double sum = 0;
    for (auto temp : partialSums) {
        sum += temp;
    }

    double gamma = sum - log(n);

    return gamma;
}


int main() {
    cout <<"Server started...\n";

    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        std::cerr << "Can't initialize Winsock! Quitting" << std::endl;
        return 1;
    }

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        std::cerr << "Can't create socket! Quitting" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    listen(listening, SOMAXCONN);

    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Can't accept client connection! Quitting" << std::endl;
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    int n,p;
    recv(clientSocket, (char*)&n, sizeof(int), 0);
    recv(clientSocket, (char*)&p, sizeof(int), 0);

    cout << "Received data from client: n = " << n << ", p = " << p << "\n";

    double result = calcGamma(n, p);

    send(clientSocket, (char*)&result, sizeof(double), 0);

    closesocket(clientSocket);
    closesocket(listening);

    WSACleanup();

    cout << "Closing server...\n";

    return 0;
}