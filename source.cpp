#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

int send_message(char* message)
{
    WSADATA              wsaData;
    SOCKET               SendingSocket;
    SOCKADDR_IN          ReceiverAddr, SrcInfo;
    int                  Port = 5150;
    char* SendBuf = message;
    int                  BufLength = 1024;
    int len;
    int TotalByteSent;

    // Initialize Winsock version 2.2
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Client: WSAStartup failed with error %ld\n", WSAGetLastError());
        // Clean up
        WSACleanup();
        // Exit with error
        return -1;
    }
    else
        printf("Client: The Winsock DLL status is %s.\n", wsaData.szSystemStatus);

    // Create a new socket to receive datagrams on.
    SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (SendingSocket == INVALID_SOCKET)
    {
        printf("Client: Error at socket(): %ld\n", WSAGetLastError());
        // Clean up
        WSACleanup();
        // Exit with error
        return -1;
    }
    else
        printf("Client: socket() is OK!\n");

    // Set up a SOCKADDR_IN structure that will identify who we
    // will send datagrams to. For demonstration purposes, let's
    // assume our receiver's IP address is 127.0.0.1 and waiting
    // for datagrams on port 5150.
    ReceiverAddr.sin_family = AF_INET;
    ReceiverAddr.sin_port = htons(Port);
    ReceiverAddr.sin_addr.s_addr = inet_addr("8.8.8.8");

    // Send a datagram to the receiver.
    printf("Client: Data to be sent: \"%s\"\n", SendBuf);
    printf("Client: Sending datagrams...\n");
    TotalByteSent = sendto(SendingSocket, SendBuf, BufLength, 0,
        (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));

    printf("Client: sendto() looks OK!\n");

    // Some info on the receiver side...
    // Allocate the required resources
    memset(&SrcInfo, 0, sizeof(SrcInfo));
    len = sizeof(SrcInfo);

    getsockname(SendingSocket, (SOCKADDR*)&SrcInfo, &len);
    printf("Client: Sending IP(s) used: %s\n", inet_ntoa(SrcInfo.sin_addr));
    printf("Client: Sending port used: %d\n", htons(SrcInfo.sin_port));

    // Some info on the sender side
    getpeername(SendingSocket, (SOCKADDR*)&ReceiverAddr, (int*)sizeof(ReceiverAddr));
    printf("Client: Receiving IP used: %s\n", inet_ntoa(ReceiverAddr.sin_addr));
    printf("Client: Receiving port used: %d\n", htons(ReceiverAddr.sin_port));
    printf("Client: Total byte sent: %d\n", TotalByteSent);

    // When your application is finished receiving datagrams close the socket.
    printf("Client: Finished sending. Closing the sending socket...\n");
    if (closesocket(SendingSocket) != 0)
        printf("Client: closesocket() failed! Error code: %ld\n", WSAGetLastError());
    else
        printf("Server: closesocket() is OK\n");

    // When your application is finished call WSACleanup.
    printf("Client: Cleaning up...\n");
    if (WSACleanup() != 0)
        printf("Client: WSACleanup() failed! Error code: %ld\n", WSAGetLastError());
    else
        printf("Client: WSACleanup() is OK\n");
    // Back to the system
    return 0;
}

static std::string GetProductName()
{
	std::string ret;
	char value[64];
	DWORD size = _countof(value);
	DWORD type = REG_SZ;
	HKEY key;
	LONG retKey = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ | KEY_WOW64_64KEY, &key);
	LONG retVal = ::RegQueryValueExA(key, "ProductName", nullptr, &type, (LPBYTE)value, &size);
	if (retKey == ERROR_SUCCESS && retVal == ERROR_SUCCESS) {
		ret = value;
	}
	::RegCloseKey(key);
	return ret;
}


static std::string GetMachineGUID()
{
	std::string ret;
	char value[64];
	DWORD size = _countof(value);
	DWORD type = REG_SZ;
	HKEY key;
	LONG retKey = ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ | KEY_WOW64_64KEY, &key);
	LONG retVal = ::RegQueryValueExA(key, "MachineGuid", nullptr, &type, (LPBYTE)value, &size);
	if (retKey == ERROR_SUCCESS && retVal == ERROR_SUCCESS) {
		ret = value;
	}
	::RegCloseKey(key);
	return ret;
}

int main() {
	std::string machine_guid;
	std::string product_name;
	std::string complete;

	machine_guid = GetMachineGUID();
	product_name = GetProductName();
	
	complete = machine_guid + "\n" + product_name;

	std::cout << complete;
    send_message(const_cast<char*>(complete.c_str()));

	return 0;
}
