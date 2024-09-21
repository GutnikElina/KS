#include <iostream>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib") 
#pragma warning(disable:4996)

using namespace std;

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);

    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        cout << "Error" << endl;
        exit(1);
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(1280);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        cout << "Error: Connection Failed" << endl;
        return -1;
    }

    char word[255];
    cout << "Enter a word: ";
    cin >> word;

    if (send(s, word, strlen(word), 0) == SOCKET_ERROR)
    {
        cout << "Error sending data" << endl;
        closesocket(s);
        WSACleanup();
        return -1;
    }

    char reversedWord[255];
    int bytesReceived = recv(s, reversedWord, sizeof(reversedWord), 0);

    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "Error receiving data" << endl;
        closesocket(s);
        WSACleanup();
        return -1;
    }
    reversedWord[bytesReceived] = '\0';

    cout << "Reversed word from server: " << reversedWord << endl;

    closesocket(s);
    WSACleanup();
    return 0;
}
