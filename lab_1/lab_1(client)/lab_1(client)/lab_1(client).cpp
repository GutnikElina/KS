#include <iostream>
#include <winSock2.h>
#include <string>
#include <vector>
#include <windows.h>

#pragma warning(disable:4996)
#pragma comment (lib, "ws2_32.lib")

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
};

class Service
{
public:

    static void add(SOCKET);
    static void show(SOCKET);
    static void edit(SOCKET);
    static void deleteStudent(SOCKET);
    static void find(SOCKET);
};

void Service::add(SOCKET s)
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

    send(s, name, sizeof(name), 0);
    send(s, (char*)&groupNumber, sizeof(groupNumber), 0);
    send(s, (char*)&scholarship, sizeof(scholarship), 0);
    send(s, (char*)&averageGrade, sizeof(averageGrade), 0);
}

void Service::show(SOCKET s)
{
    int numStudents;
    recv(s, (char*)&numStudents, sizeof(numStudents), 0);

    cout << endl << "--------  СПИСОК СТУДЕНТОВ --------" << endl;
    for (int i = 0; i < numStudents; ++i)
    {
        Student student;
        recv(s, (char*)&student, sizeof(student), 0);

        cout << " ------ Студент №" << i + 1 << ":" << endl;
        cout << "ФИО: " << student.getName() << endl;
        cout << "Номер группы: " << student.getGroupNumber() << endl;
        cout << "Стипендия: " << student.getScholarship() << endl;
        cout << "Средний балл: " << student.getAverageGrade() << endl;
        cout << "-----------------------------------" << endl;
    }
}

void Service::edit(SOCKET s)
{
    show(s);
    cout << endl << " --> Введите номер студента: ";
    int choice;
    cin >> choice;
    send(s, (char*)&choice, sizeof(choice), 0);

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

    send(s, name, sizeof(name), 0);
    send(s, (char*)&groupNumber, sizeof(groupNumber), 0);
    send(s, (char*)&scholarship, sizeof(scholarship), 0);
    send(s, (char*)&averageGrade, sizeof(averageGrade), 0);

    show(s);
}

void Service::deleteStudent(SOCKET s)
{
    show(s);
    cout << endl << " --> Введите номер студента, которого хотите удалить: ";
    int choice;
    cin >> choice;
    send(s, (char*)&choice, sizeof(choice), 0);
    show(s);
}

void Service::find(SOCKET s)
{
    float averageGrade;
    cout << endl << " --> Введите средний балл: ";
    cin >> averageGrade;

    send(s, (char*)&averageGrade, sizeof(averageGrade), 0);

    int numFilteredStudents;
    recv(s, (char*)&numFilteredStudents, sizeof(numFilteredStudents), 0);

    cout << endl << "НАЙДЕНО СТУДЕНТОВ: " << numFilteredStudents << endl;

    cout << endl << "--------  СПИСОК СТУДЕНТОВ --------" << endl;
    for (int i = 0; i < numFilteredStudents; i++)
    {
        Student student;
        recv(s, (char*)&student, sizeof(student), 0);

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
        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
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

        if (connect(s, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            cout << "-!!!- Failed to connect to server -!!!-" << endl;
            closesocket(s);
            WSACleanup();
            return 1;
        }

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

        send(s, choice, sizeof(choice), 0);

        char ch = choice[0];
        switch (ch)
        {
        case '1':
            Service::add(s);
            break;
        case '2':
            Service::show(s);
            break;
        case '3':
            Service::edit(s);
            break;
        case '4':
            Service::deleteStudent(s);
            break;
        case '5':
            Service::find(s);
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