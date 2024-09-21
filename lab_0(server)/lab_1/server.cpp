#include <iostream>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib") 
#pragma warning(disable:4996)

using namespace std;

int main()
{
	WORD wVersionRequested;
	WSADATA wsaData;                    
	wVersionRequested = MAKEWORD(2,2);   // ������������� ������ ���������� WinSock, �����, ����� ��������� ����������

	if (WSAStartup(wVersionRequested, &wsaData) != 0) // ������� ������������ ��� ������������� ����������
		//1 ��������-������������� ������ ����������, 2-������ �� ��������� wsaData
	{
		cout << "Error" << endl;
		exit(1);
	}

	// ��������� ���������� �� ������ ������

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in local;          // ��� ��������� ������������� ��� �������� ������, IN ��� ��������-����������
	int sizeoflocal = sizeof(local);
	local.sin_family = AF_INET;        // ��������� ���������� 
	local.sin_port = htons(1280);      // ���� ��� ������������� ��������� ������������ ������� 
	local.sin_addr.s_addr = htonl(INADDR_ANY);      // �������� ���������� IP-����� ����������
	int c = bind(s,(struct sockaddr*)&local, sizeoflocal);    // ��� �������� ������ ������
  // 1 �������������� ��������� �����, 2 ��������� �� ��������� SOCKADDR_, 3  ������ ���� ���������

	int r = listen(s, SOMAXCONN);    // ������� ��� ������������� ����� � �������� ���������� �� ������� �������, 
  // 2 �������� - ����������� ���������� ����� ��������, ��������� ���������

	// ����� �����, ����� ���������� ��������� � ��������
	SOCKET newConnection;
	newConnection = accept(s, (struct sockaddr*)&local, &sizeoflocal);
	// 1 �������� - ��� ������ ��� ��������� � ���������� ��� ������������� �����
    //2 ��������� �� ��������� ���� SOCKADDR, 3 ������ �� ������ ��������� SOCKADDR

	if (newConnection == 0)
	{
		cout << "Error 2\n" << endl;
	}
	else
	{
		cout << "Server: Client Connected\n";


		//��������� ����� �� ������� � �������� ��������� �����


		char receivedWord[255];
		int bytesReceived = recv(newConnection, receivedWord, sizeof(receivedWord), 0);

		if (bytesReceived == SOCKET_ERROR)
		{
			cout << "Error receiving data" << endl;
			closesocket(newConnection);
			closesocket(s);
			WSACleanup();
			return -1;
		}

		receivedWord[bytesReceived] = '\0';
		cout << "Received word from client: " << receivedWord << endl;

		int sizeofReceivedWord = strlen(receivedWord);
		char reversedWord[255];
		int j = 0;
		for (int i = sizeofReceivedWord - 1; i >= 0; i--)
		{
			reversedWord[j++] = receivedWord[i];
		}
		reversedWord[j] = '\0'; 

		cout << "Reversed word: " << reversedWord << endl;

		if (send(newConnection, reversedWord, strlen(reversedWord), 0) == SOCKET_ERROR)
		{
			cout << "Error sending data" << endl;
			closesocket(newConnection);
			closesocket(s);
			WSACleanup();
			return -1;
		}

		closesocket(newConnection);
	}

	// �������� ���������� � ������������ �������� 
	closesocket(s);
	WSACleanup();
	return 0;
}