#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<winsock2.h>
#include<windows.h>
#include<process.h>
#include<string>
#include<vector>
#include<fstream>

#pragma comment(lib, "Ws2_32.lib") 
using namespace std;

class Student
{
    char name[255];
    int groupNumber;
    float scholarship;
    float averageGrade;

public:

    Student()
    {
        strncpy(name, "Студент", sizeof(name));
        groupNumber = 0;
        scholarship = 0;
        averageGrade = 0;
    }

    Student(const char* name, int groupNumber, float scholarship, float averageGrade)
    {
        strncpy(this->name, name, sizeof(this->name));
        this->groupNumber = groupNumber;
        this->scholarship = scholarship;
        this->averageGrade = averageGrade;
    }

    const char* getName() { return this->name; }
    int getGroupNumber() { return this->groupNumber; }
    float getScholarship() { return this->scholarship; }
    float getAverageGrade() { return this->averageGrade; }

    void setName(const char* name) { strncpy(this->name, name, sizeof(this->name)); }
    void setGroupNumber(int groupNumber) { this->groupNumber = groupNumber; }
    void setScholarship(float scholarship) { this->scholarship = scholarship; }
    void setAverageGrade(float averageGrade) { this->averageGrade = averageGrade; }
};

class Server
{
public:

    static void add(SOCKET, struct sockaddr_in, int);
    static void show(SOCKET, struct sockaddr_in, int);
    static void edit(SOCKET, struct sockaddr_in, int);
    static void deleteStudent(SOCKET, struct sockaddr_in, int);
    static void find(SOCKET, struct sockaddr_in, int);
};

void Server::add(SOCKET s, struct sockaddr_in ad, int l)
{
    char name[255];
    int groupNumber;
    float scholarship, averageGrade, grade_1, grade_2, grade_3;

    cout << endl << "-------- ДОБАВЛЕНИЕ СТУДЕНТА --------";
    cout << endl << " ФИО: ";
    cin >> name;
    cout << endl << " Номер группы: ";
    cin >> groupNumber;
    cout << endl << " Стипендия: ";
    cin >> scholarship;
    cout << endl << " Первая оценка: ";
    cin >> grade_1;
    cout << endl << " Вторая оценка: ";
    cin >> grade_2;
    cout << endl << " Третья оценка: ";
    cin >> grade_3;
    averageGrade = (grade_1 + grade_2 + grade_3) / 3;

    sendto(s, name, sizeof(name), 0, (struct sockaddr*)&ad, l);
    sendto(s, (char*)&groupNumber, sizeof(groupNumber), 0, (struct sockaddr*)&ad, l);
    sendto(s, (char*)&scholarship, sizeof(scholarship), 0, (struct sockaddr*)&ad, l);
    sendto(s, (char*)&averageGrade, sizeof(averageGrade), 0, (struct sockaddr*)&ad, l);
}

void Server::show(SOCKET s, struct sockaddr_in ad, int l)
{
    int numStudents;
    recvfrom(s, (char*)&numStudents, sizeof(numStudents), 0, (struct sockaddr*)&ad, &l);

    cout << endl << "--------  СПИСОК СТУДЕНТОВ --------" << endl;
    for (int i = 0; i < numStudents; ++i)
    {
        Student student;
        recvfrom(s, (char*)&student, sizeof(student), 0, (struct sockaddr*)&ad, &l);

        cout << " ------ Студент №" << i + 1 << ":" << endl;
        cout << "ФИО: " << student.getName() << endl;
        cout << "Номер группы: " << student.getGroupNumber() << endl;
        cout << "Стипендия: " << student.getScholarship() << endl;
        cout << "Средний балл: " << student.getAverageGrade() << endl;
        cout << "-----------------------------------" << endl;
    }
}

void Server::edit(SOCKET s, struct sockaddr_in ad, int l)
{
    Server::show(s, ad, l);
    cout << endl << " --> Введите номер студента: ";
    int choice;
    cin >> choice;
    sendto(s, (char*)&choice, sizeof(choice), 0, (struct sockaddr*)&ad, l);

    char name[255];
    int groupNumber;
    float scholarship, averageGrade, grade_1, grade_2, grade_3;

    cout << endl << "-------- ИЗМЕНЕНИЕ СТУДЕНТА --------";
    cout << endl << " ФИО: ";
    cin >> name;
    cout << endl << " Номер группы: ";
    cin >> groupNumber;
    cout << endl << " Стипендия: ";
    cin >> scholarship;
    cout << endl << " Первая оценка: ";
    cin >> grade_1;
    cout << endl << " Вторая оценка: ";
    cin >> grade_2;
    cout << endl << " Третья оценка: ";
    cin >> grade_3;
    averageGrade = (grade_1 + grade_2 + grade_3) / 3;

    sendto(s, name, sizeof(name), 0, (struct sockaddr*)&ad, l);
    sendto(s, (char*)&groupNumber, sizeof(groupNumber), 0, (struct sockaddr*)&ad, l);
    sendto(s, (char*)&scholarship, sizeof(scholarship), 0, (struct sockaddr*)&ad, l);
    sendto(s, (char*)&averageGrade, sizeof(averageGrade), 0, (struct sockaddr*)&ad, l);

    Server::show(s, ad, l);
}

void Server::deleteStudent(SOCKET s, struct sockaddr_in ad, int l)
{
    Server::show(s, ad, l);
    cout << endl << " --> Введите номер студента, которого хотите удалить: ";
    int choice;
    cin >> choice;
    sendto(s, (char*)&choice, sizeof(choice), 0, (struct sockaddr*)&ad, l);
    Server::show(s, ad, l);
}

void Server::find(SOCKET s, struct sockaddr_in ad, int l)
{
    float averageGrade;
    cout << endl << " --> Введите средний балл: ";
    cin >> averageGrade;

    sendto(s, (char*)&averageGrade, sizeof(averageGrade), 0, (struct sockaddr*)&ad, l);

    int numFilteredStudents;
    recvfrom(s, (char*)&numFilteredStudents, sizeof(numFilteredStudents), 0, (struct sockaddr*)&ad, &l);

    cout << endl << "НАЙДЕНО СТУДЕНТОВ: " << numFilteredStudents << endl;

    cout << endl << "--------  СПИСОК СТУДЕНТОВ --------" << endl;
    for (int i = 0; i < numFilteredStudents; i++)
    {
        Student student;
        recvfrom(s, (char*)&student, sizeof(student), 0, (struct sockaddr*)&ad, &l);

        cout << " ------ Студент №" << i + 1 << ":" << endl;
        cout << "ФИО: " << student.getName() << endl;
        cout << "Номер группы: " << student.getGroupNumber() << endl;
        cout << "Стипендия: " << student.getScholarship() << endl;
        cout << "Средний балл: " << student.getAverageGrade() << endl;
        cout << "-----------------------------------" << endl;
    }
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    int err;
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        cout << "-!!!- WSAStartup failed with error -!!!-" << endl;
        return 1;
    }

    while (true)
    {
        SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s == INVALID_SOCKET)
        {
            cout << "-!!!- Socket error -!!!-" << endl;
            WSACleanup();
            return 1;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(1280);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        int len = sizeof(serverAddr);

        cout << endl << "CONNECTED TO SERVER" << endl;

        char choice[5];
        cout << "-------------- МЕНЮ ---------------" << endl;
        cout << "| 1) Добавление                   |" << endl;
        cout << "| 2) Просмотр                     |" << endl;
        cout << "| 3) Редактирование               |" << endl;
        cout << "| 4) Удаление                     |" << endl;
        cout << "| 5) Поиск студентов по ср. баллу |" << endl;
        cout << "| 6) Выход                        |" << endl;
        cout << "-----------------------------------" << endl;
        cout << " -> Ваш выбор: ";
        cin >> choice;

        sendto(s, choice, sizeof(choice), 0, (struct sockaddr*)&serverAddr, len);

        char ch = choice[0];
        switch (ch)
        {
        case '1':
            Server::add(s, serverAddr, len);
            break;
        case '2':
            Server::show(s, serverAddr, len);
            break;
        case '3':
            Server::edit(s, serverAddr, len);
            break;
        case '4':
            Server::deleteStudent(s, serverAddr, len);
            break;
        case '5':
            Server::find(s, serverAddr, len);
            break;
        case '6':
            return 0;
        default:
            cout << endl << "Неправильный ввод! Попробуйте снова" << endl;
            break;
        }
        closesocket(s);
    }
    WSACleanup();
    return 0;
}
