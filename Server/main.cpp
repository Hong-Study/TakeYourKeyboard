#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;
#define PORT 3600
#define IP "127.0.0.1"

void HandleError(const char* cause) {
    int errCode = ::WSAGetLastError();
    std::cout << cause << "Error Code : " << errCode << std::endl;
}

void NetWorking_Init();

SOCKET listen_s;
SOCKADDR_IN sock_addr;

int main() {

    NetWorking_Init();

    SOCKADDR_IN client_addr;
    SOCKET client_s;

    ZeroMemory(&client_addr, sizeof(struct sockaddr_in));

    int nAcceptClientInfo = sizeof(struct sockaddr_in);
    client_s = accept(listen_s, (struct sockaddr*)&client_addr, &nAcceptClientInfo);
    int nReceiveBytes = 0;
    printf("New Client Connected : %s\n", inet_ntoa(client_addr.sin_addr));
    
    DWORD buffer;
    int recvLen = 0;
    while (1) {
        recvLen = recv(client_s, reinterpret_cast<char*>(&buffer), sizeof(DWORD), 0);
        if (recvLen == 0)
            break;
        cout << "Data : " << buffer << endl;
    }
    closesocket(client_s);
    closesocket(listen_s);
    WSACleanup();
}

void NetWorking_Init()
{
    WSADATA wsaData;
    SOCKADDR_IN sock_addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        HandleError("WSAStartup");

    listen_s = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_s == INVALID_SOCKET)
        HandleError("socket");

    ZeroMemory(&sock_addr, sizeof(struct sockaddr_in));

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(PORT);
    sock_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    if (bind(listen_s, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
        HandleError("Bind");
        return;
    }
    if (listen(listen_s, 5) == SOCKET_ERROR)
    {
        HandleError("listen");
        return;
    }
}
