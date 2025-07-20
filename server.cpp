#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#define DEFAULT_PORT "27015"


#define SERVERLOG(x) do { std::cout << "SERVER: " << x << std::endl; }while(0)


int main()
{

    WSADATA wsaData;
    
    {
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if(result != 0)
        {
            SERVERLOG("Failed to initialize winsocket");
            return -1;
        }
    }

    // Get the address info for a socket we want to create
    struct addrinfo* addr_result = nullptr;
    struct addrinfo* ptr = nullptr;
    struct addrinfo hints  = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;    // SOCK_DRGAM
    hints.ai_protocol = IPPROTO_UDP;    // IPPROTO_UDP
    hints.ai_flags = AI_PASSIVE;
    {
        int result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addr_result);
        if(result != 0)
        {
            SERVERLOG("Failed to get addr info");
            WSACleanup();
            return -1;
        }
    }



    // Create the socket
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);

    if(ListenSocket == INVALID_SOCKET)
    {
        SERVERLOG("Error at socket(): " + WSAGetLastError());
        freeaddrinfo(addr_result);
        WSACleanup();
        return -1;
    }


    // Actually bind the socket to the ip and port we set in getaddrinfo
    {
        // NOTE: addr_result->ai_addr is the struct sockaddr_in that we are binding to. It holds the ip and port that we are actually binding to
        int result = bind(ListenSocket, addr_result->ai_addr, (int)addr_result->ai_addrlen);
        if(result == SOCKET_ERROR)
        {
            SERVERLOG("Failed to bind socket: " + WSAGetLastError());
            freeaddrinfo(addr_result);
            closesocket(ListenSocket);
            WSACleanup();
            return -1;
        }

        freeaddrinfo(addr_result);
    }

    // This is for tcp stream sockets only
    /*if(listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        SERVERLOG("Faild to listen on ip and port: " + WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return -1;
    }

    SERVERLOG("Awaiting Connection...");

    // Accept the client
    SOCKET client = accept(ListenSocket, nullptr, nullptr);
    if(client == INVALID_SOCKET)
    {
        SERVERLOG("accept failed: " + WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return -1;
    }

    // Normally would keep this around on this thread but don't need this for this example so closing it
    closesocket(ListenSocket);


    SERVERLOG("Connected to client...");
    */

    SERVERLOG("Waiting for data...");
    char recvbuf[512];

    // TCP
    /*if(int bytes_recieved = recv(client, recvbuf, 512, 0) > 0)
    {
        SERVERLOG(bytes_recieved);
        SERVERLOG(recvbuf << " Connected");
    }*/

    struct sockaddr_in client;
    int client_size = sizeof(struct sockaddr_in);

    if(int bytes_recieved = recvfrom(ListenSocket, recvbuf, 512, 0, (sockaddr*)&client, &client_size) > 0)
    {
        SERVERLOG("Connection Recieved...");
        recvbuf[bytes_recieved] = '\0';
        SERVERLOG("[ " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << "] " << recvbuf);
    }


    SERVERLOG("Shutting Down...");

    // TCP
    /*int result = shutdown(client, SD_SEND);
    if(result == SOCKET_ERROR)
    {
        SERVERLOG("shutdown failed: " + WSAGetLastError());
        closesocket(client);
        WSACleanup();
        return -1;
    }

    closesocket(client);
    */

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}