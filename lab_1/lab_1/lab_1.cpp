#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <thread> 
#include <vector>
#include <windows.h>
#include <fstream>
#include <cstring>

#pragma warning(disable:4996)
#pragma comment (lib, "ws2_32.lib")

using namespace std;

int numcl = 0;

void print()
{
    if (numcl)
        cout << numcl << " client connected!" << endl;
    else cout << "No clients connected!" << endl;
}

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

class Service
{
public:

    static void add(SOCKET);
    static void show(SOCKET);
    static void edit(SOCKET);
    static void deleteStudent(SOCKET);
    static void findByAverageGrade(SOCKET);
    static vector<Student> filterStudents(float);
    static void addInFile(Student);
    static void getFromFile(vector<Student>&);
    static int countStudents();
};

int Service::countStudents()
{
    ifstream in;
    in.open("students.txt", ios::in);
    string str;
    int i = 0;
    if (in.is_open())
    {
        while (getline(in, str))
        {
            i++;
        }
    }
    else
        cout << "-!!!- Error with opening file -!!!-" << endl;
    in.close();
    return i;
}

void Service::addInFile(Student student)
{
    ofstream out("students.txt", ios::app);
    if (out.is_open())
    {
        out << student.getName() << " " << student.getGroupNumber() << " " << student.getScholarship() << " " << student.getAverageGrade() << endl;
        out.close();
    }
    else
        cout << "-!!!- Error with opening file -!!!-" << endl;
}

void Service::getFromFile(vector<Student>& students)
{
    ifstream in("students.txt", ios::in);
    if (in.is_open())
    {
        string name;
        int groupNumber;
        float scholarship, averageGrade;
        while (in >> name >> groupNumber >> scholarship >> averageGrade)
        {
            Student p(name.c_str(), groupNumber, scholarship, averageGrade);
            students.push_back(p);
        }
        in.close();
    }
    else
        cout << "-!!!- Error with opening file -!!!-" << endl;
}

void Service::add(SOCKET s2)
{
    char name[255];
    int groupNumber;
    float scholarship, averageGrade;

    recv(s2, name, sizeof(name), 0);
    recv(s2, (char*)&groupNumber, sizeof(groupNumber), 0);
    recv(s2, (char*)&scholarship, sizeof(scholarship), 0);
    recv(s2, (char*)&averageGrade, sizeof(averageGrade), 0);

    Student student = { name, groupNumber, scholarship, averageGrade };
    addInFile(student);
}

void Service::show(SOCKET s2)
{
    int numStudents = countStudents();
    send(s2, (char*)&numStudents, sizeof(numStudents), 0);

    vector<Student> students;
    students.reserve(countStudents());
    getFromFile(students);

    for (const auto& student : students)
    {
        send(s2, (char*)&student, sizeof(student), 0);
    }
}

void Service::edit(SOCKET s2)
{
    int choice;
    show(s2);
    recv(s2, (char*)&choice, sizeof(choice), 0);
    choice--;

    char name[255];
    int groupNumber;
    float scholarship, averageGrade;

    vector<Student> students;
    students.reserve(countStudents());
    getFromFile(students);

    recv(s2, name, sizeof(name), 0);
    recv(s2, (char*)&groupNumber, sizeof(groupNumber), 0);
    recv(s2, (char*)&scholarship, sizeof(scholarship), 0);
    recv(s2, (char*)&averageGrade, sizeof(averageGrade), 0);

    for (int i = 0; i < students.size(); i++)
    {
        if (i == choice)
        {
            students[i].setName(name);
            students[i].setGroupNumber(groupNumber);
            students[i].setScholarship(scholarship);
            students[i].setAverageGrade(averageGrade);
        }
    }

    ofstream out("students.txt", ios::out);
    for (int i = 0; i < students.size(); i++)
    {
        addInFile(students[i]);
    }

    out.close();

    show(s2);
}

void Service::deleteStudent(SOCKET s2)
{
    int choice;
    show(s2);
    recv(s2, (char*)&choice, sizeof(choice), 0);
    choice--;

    vector<Student> students;
    students.reserve(countStudents());
    getFromFile(students);

    if (choice >= 0 && choice < students.size()) 
    {
        students.erase(students.begin() + choice);

        ofstream out("students.txt", ios::out);
        for (int i = 0; i < students.size(); i++)
        {
            addInFile(students[i]);
        }
        out.close();
    }

    show(s2);
}

vector<Student> Service::filterStudents(float averageGrade)
{
    vector<Student> students;
    students.reserve(countStudents());
    getFromFile(students);

    vector<Student> filteredStudents;
    for (size_t i = 0; i < students.size(); ++i)
    {
        if (students.at(i).getAverageGrade() > averageGrade)
            filteredStudents.push_back(students.at(i));
    }
    return filteredStudents;
}

void Service::findByAverageGrade(SOCKET s2)
{
    float averageGrade = 0;
    recv(s2, (char*)&averageGrade, sizeof(averageGrade), 0);

    vector<Student> filteredStudents = filterStudents(averageGrade);

    int numFilteredStudents = filteredStudents.size();
    send(s2, (char*)&numFilteredStudents, sizeof(numFilteredStudents), 0);

    for (const auto& student : filteredStudents)
    {
        send(s2, (char*)&student, sizeof(student), 0);
    }
    closesocket(s2); 
}

DWORD WINAPI ThreadFunc(LPVOID client_socket)
{
    SOCKET s2 = *((SOCKET*)client_socket);
    char buf[10];
    while (recv(s2, buf, sizeof(buf), 0) > 0)
    {
        char choice = buf[0];
        switch (choice)
        {
        case '1':
            Service::add(s2);
            break;
        case '2':
            Service::show(s2);
            break;
        case '3':
            Service::edit(s2);
            break;
        case '4':
            Service::deleteStudent(s2);
            break;
        case '5':
            Service::findByAverageGrade(s2);
            break;
        }
    }
    closesocket(s2);
    return 0;
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    WSADATA wsaData;  //структура wsaData – содержит номер версии, которая должна использоваться (поле wVersion), максимальный номер версии, поддерживаемый данной библиотекой 
    //(поле wHighVersion), текстовые строки с описанием реализации WinSock, максимальное число сокетов, доступных процессу и 
    // максимально допустимый размер дейтаграмм.
    WORD wVersionRequested = MAKEWORD(2, 2);         //значение типа word, которое определяет максимальный номер версии WinSock, доступный приложению
    int error = WSAStartup(wVersionRequested, &wsaData);   //для того чтобы можно было использовать интерфейс программирования WinSock
    if (error != 0)
    {
        return 1;
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);     //создание сокета. AF_INET - означает, что с этим сокетом будут 
    //использоваться адреса Интернет; следующие два аргумента задают тип создаваемого сокета и протокол обмена данными через него

    if (s == INVALID_SOCKET)
    {
        WSACleanup();
        cout << endl << "-!!!- Socket error -!!!-" << endl;
        return 1;
    }

    sockaddr_in local_addr;             // эта структура предназначена для хранения адреса, IN для интернет-протоколов
    local_addr.sin_family = AF_INET;    // семейство протоколов 
    local_addr.sin_port = htons(1280);  // порт для идентификации программы поступающими данными 
    local_addr.sin_addr.s_addr = INADDR_ANY;   // хранится физический IP-адрес компьютера

    // для связывания конкретного адреса с сокетом
    // 1 предварительно созданный сокет, 2 указатель на структуру SOCKADDR_, 3  размер этой структуры
    if (bind(s, (sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR)
    {
        closesocket(s);
        WSACleanup();
        cout << "-!!!- Binding failed -!!!-" << endl;
        return 1;
    }

    // функция для прослушивания порта в ожидании соединения со стороны клиента, 
    // 2 параметр - максимально допустимое число запросов, ожидающих обработки
    if (listen(s, 5) == SOCKET_ERROR)
    {
        closesocket(s);
        WSACleanup();
        cout << "-!!!- Listening failed -!!!-" << endl;
        return 1;
    }

    cout << "---SERVER IS ONLINE---" << endl;

    //максимальное количество подключений
    int availableConnections = 10;

    while (availableConnections--)
    {
        SOCKET client_socket; // сокет для клиента 
        sockaddr_in client_addr;  //адрес клиента (заполняется системой)
        int client_addr_size = sizeof(client_addr);

        // при получении запроса клиента открытие соединения
        client_socket = accept(s, (sockaddr*)&client_addr, &client_addr_size);
        // 1 параметр - это только что созданный и запущенный для прослушивания сокет
        // 2 указатель на структуру типа SOCKADDR, 3 ссылка на размер структуры SOCKADDR
        if (client_socket == INVALID_SOCKET)
        {
            cout << "-!!!- Accept failed -!!!-" << endl;
            continue;
        }

        numcl++;
        print();

        // вызов нового потока для обслуживания клиента 
        DWORD dwThreadId;
        // функция CreateThread() создает поток, который выполняется в пределах адресного пространства вызова процесса
        CreateThread(NULL, 0, ThreadFunc, &client_socket, 0, &dwThreadId);
        // 1 - указатель на атрибуты безопасности (если 0, то не может быть унаследован дочерними процессами)
        // 2 - размер стека начального потока (0 - используется тот же размер, что и при вызове потока)
    }   // 3 - указатель на определенную прикладную функцию; для выполнения ее потоком и представления начального адреса потока.
        // 4 - определяет единственную 32-битовую величину параметра, переданную в поток
        // 5 - используется для определения дополнительных флагов, которые управляют созданием потока
        // 6 - указатель на 32-битовую переменную, которая получает идентификатор потока.

    closesocket(s);
    WSACleanup();
}
