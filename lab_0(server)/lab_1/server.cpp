#include <iostream>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib") 
#pragma warning(disable:4996)

using namespace std;

int main()
{
	WORD wVersionRequested;
	WSADATA wsaData;                    
	wVersionRequested = MAKEWORD(2,2);   // запрашиваемая версия библиотеки WinSock, нужна, чтобы загрузить библиотеку

	if (WSAStartup(wVersionRequested, &wsaData) != 0) // функция используется для инициализации библиотеки
		//1 параметр-запрашиваемая версия библиотеки, 2-ссылка на структуру wsaData
	{
		cout << "Error" << endl;
		exit(1);
	}

	// заполняем информацию об адресе сокета

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in local;          // эта структура предназначена для хранения адреса, IN для интернет-протоколов
	int sizeoflocal = sizeof(local);
	local.sin_family = AF_INET;        // семейство протоколов 
	local.sin_port = htons(1280);      // порт для идентификации программы поступающими данными 
	local.sin_addr.s_addr = htonl(INADDR_ANY);      // хранится физический IP-адрес компьютера
	int c = bind(s,(struct sockaddr*)&local, sizeoflocal);    // для привязки адреса сокета
  // 1 предварительно созданный сокет, 2 указатель на структуру SOCKADDR_, 3  размер этой структуры

	int r = listen(s, SOMAXCONN);    // функция для прослушивания порта в ожидании соединения со стороны клиента, 
  // 2 параметр - максимально допустимое число запросов, ожидающих обработки

	// новый сокет, чтобы удерживать отношения с клиентом
	SOCKET newConnection;
	newConnection = accept(s, (struct sockaddr*)&local, &sizeoflocal);
	// 1 параметр - это только что созданный и запущенный для прослушивания сокет
    //2 указатель на структуру типа SOCKADDR, 3 ссылка на размер структуры SOCKADDR

	if (newConnection == 0)
	{
		cout << "Error 2\n" << endl;
	}
	else
	{
		cout << "Server: Client Connected\n";


		//получение слова от клиента и отправка обратного слова


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

	// закрытие соединения и освобождение ресурсов 
	closesocket(s);
	WSACleanup();
	return 0;
}