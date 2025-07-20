#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#define CLIENTLOG(x) do { std::cout << "CLIENT: " << x << std::endl; }while(0)
#define DEFAULT_PORT "27015"

int main(int argc, char* argv[])
{

    if(argc < 2)
    {
        std::cout << "Error: Incorrect Usage" << std::endl;
        std::cout << "Correct Usage: ./client [server_ip]" << std::endl;
    }

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(result != 0)
    {
        CLIENTLOG("Failed to initialize winsock");
        return -1;
    }

    struct addrinfo* addr_result = nullptr;
    struct addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM; // SOCK_DGRAM
    hints.ai_protocol = IPPROTO_UDP; // IPPORTO_UDP

    result = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &addr_result);
    if(result != 0)
    {
        CLIENTLOG("getaddrinfo failed: " + result);
        WSACleanup();
    }

    SOCKET connect_socket = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
    if(connect_socket == INVALID_SOCKET)
    {
        CLIENTLOG("Error at socket(): " + +WSAGetLastError());
        freeaddrinfo(addr_result);
        WSACleanup();
        return -1;
    }
    CLIENTLOG("Initialized...");

    /*
    CLIENTLOG("Trying to connect to server...");

    result = connect(connect_socket, addr_result->ai_addr, (int)addr_result->ai_addrlen);
    if(result == SOCKET_ERROR)
    {
        closesocket(connect_socket);
        connect_socket = INVALID_SOCKET;
        return -1;
    }

    // No longer need this here if using TCP
    //freeaddrinfo(addr_result);

    if(connect_socket == INVALID_SOCKET)
    {
        CLIENTLOG("Unable to connect to server");
        WSACleanup();
        return -1;
    }


    CLIENTLOG("Connection Successful...");

    CLIENTLOG("Sending data...");

    char recvbuf[512];

    int bytes_sent = 0;
    if((bytes_sent = send(connect_socket, sendbuf, strlen(sendbuf) + 1, 0)) == SOCKET_ERROR)
    {
        CLIENTLOG("send failed: " + WSAGetLastError());
        closesocket(connect_socket);
        WSACleanup();
        return -1;
    }

    CLIENTLOG(bytes_sent);
    */

    const char sendbuf[] = "ABCD";
    std::cout << strlen(sendbuf) + 1 << std::endl;

    // NOTE: addr_result->ai_addr is the struct sockaddr_in that we would otherwise have to fill out. It holds the port and ip of the server we are trying to connect to
    if(int bytes_sent = sendto(connect_socket, "ABCD", strlen(sendbuf) + 1, 0, addr_result->ai_addr, addr_result->ai_addrlen) > 0)
    {
        CLIENTLOG("Data Sent: " << bytes_sent << " bytes...");
    }


    CLIENTLOG("Shutting Down...");
    freeaddrinfo(addr_result);
    result = shutdown(connect_socket, SD_SEND);
    if(result == SOCKET_ERROR)
    {
        CLIENTLOG("shutdown failed: " + WSAGetLastError());
        closesocket(connect_socket);
        WSACleanup();
        return -1;
    }


    closesocket(connect_socket);
    WSACleanup();
    return 0;
}