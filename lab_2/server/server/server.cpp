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

class Service
{
public:

    static void add(SOCKET, struct sockaddr_in, int);
    static void show(SOCKET, struct sockaddr_in, int);
    static void edit(SOCKET, struct sockaddr_in, int);
    static void deleteStudent(SOCKET, struct sockaddr_in, int);
    static void find(SOCKET, struct sockaddr_in, int);
    static vector<Student> filterStudents(float);
    static void writeToFile(Student);
    static void readFromFile(vector<Student>&);
    static int countStudents();
};

int Service::countStudents()
{
    ifstream in;
    in.open("student.txt", ios::in);
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

void Service::writeToFile(Student student)
{
    ofstream out("student.txt", ios::app);
    if (out.is_open())
    {
        out << student.getName() << " " << student.getGroupNumber() << " " << student.getScholarship() << " " << student.getAverageGrade() << endl;
        out.close();
    }
    else
        cout << "-!!!- Error with opening file -!!!-" << endl;
}

void Service::readFromFile(vector<Student>& students)
{
    ifstream in("student.txt", ios::in);
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


void Service::add(SOCKET s, struct sockaddr_in ad, int l)
{
    char name[255];
    int groupNumber;
    float scholarship, averageGrade;

    recvfrom(s, name, sizeof(name), 0, (struct sockaddr*)&ad, &l);
    recvfrom(s, (char*)&groupNumber, sizeof(groupNumber), 0, (struct sockaddr*)&ad, &l);
    recvfrom(s, (char*)&scholarship, sizeof(scholarship), 0, (struct sockaddr*)&ad, &l);
    recvfrom(s, (char*)&averageGrade, sizeof(averageGrade), 0, (struct sockaddr*)&ad, &l);


    Student student = { name, groupNumber, scholarship, averageGrade };
    writeToFile(student);
}

void Service::show(SOCKET s, struct sockaddr_in ad, int l)
{
    int numStudents = countStudents();
    sendto(s, (char*)&numStudents, sizeof(numStudents), 0, (struct sockaddr*)&ad, l);

    vector<Student> students;
    students.reserve(countStudents());
    readFromFile(students);

    for (const auto& student : students)
    {
        sendto(s, (char*)&student, sizeof(student), 0, (struct sockaddr*)&ad, l);
    }
}

void Service::edit(SOCKET s, struct sockaddr_in ad, int l)
{
    int choice;
    Service::show(s, ad, l);
    recvfrom(s, (char*)&choice, sizeof(choice), 0, (struct sockaddr*)&ad, &l);

    choice--;

    char name[255];
    int groupNumber;
    float scholarship, averageGrade;

    vector<Student> students;
    students.reserve(countStudents());
    readFromFile(students);

    recvfrom(s, name, sizeof(name), 0, (struct sockaddr*)&ad, &l);
    recvfrom(s, (char*)&groupNumber, sizeof(groupNumber), 0, (struct sockaddr*)&ad, &l);
    recvfrom(s, (char*)&scholarship, sizeof(scholarship), 0, (struct sockaddr*)&ad, &l);
    recvfrom(s, (char*)&averageGrade, sizeof(averageGrade), 0, (struct sockaddr*)&ad, &l);

    for (int i = 0; i < students.size(); i++)
    {
        if (i == choice)
        {
            students[i].setName(name);
            students[i].setGroupNumber(groupNumber);
            students[i].setScholarship(scholarship);
            students[i].setAverageGrade(averageGrade);
        }
        writeToFile(students[i]);
    }

    ofstream out;
    out.open("student.txt", ios::out);
    out.clear();
    out.close();

    for (int i = 0; i < students.size(); i++)
    {
        writeToFile(students[i]);
    }

    Service::show(s, ad, l);
}

void Service::deleteStudent(SOCKET s, struct sockaddr_in ad, int l)
{
    int choice;
    Service::show(s, ad, l);
    recvfrom(s, (char*)&choice, sizeof(choice), 0, (struct sockaddr*)&ad, &l);
    choice--;

    vector<Student> students;
    students.reserve(countStudents());
    readFromFile(students);

    students.erase(students.begin() + choice);

    ofstream out("student.txt", ios::out);
    out.clear();
    out.close();

    for (int i = 0; i < students.size(); i++)
    {
        writeToFile(students[i]);
    }

    Service::show(s, ad, l);
}

vector<Student> Service::filterStudents(float averageGrade)
{
    vector<Student> students;
    students.reserve(countStudents());
    readFromFile(students);

    vector<Student> filteredStudents;
    for (size_t i = 0; i < students.size(); ++i)
    {
        if (students.at(i).getAverageGrade() > averageGrade)
            filteredStudents.push_back(students.at(i));
    }
    return filteredStudents;
}

void Service::find(SOCKET s, struct sockaddr_in ad, int l)
{
    float averageGrade = 0;
    recvfrom(s, (char*)&averageGrade, sizeof(averageGrade), 0, (struct sockaddr*)&ad, &l);

    vector<Student> filteredStudents = filterStudents(averageGrade);

    int numFilteredStudents = filteredStudents.size();
    sendto(s, (char*)&numFilteredStudents, sizeof(numFilteredStudents), 0, (struct sockaddr*)&ad, l);

    for (const auto& student : filteredStudents)
    {
        sendto(s, (char*)&student, sizeof(student), 0, (struct sockaddr*)&ad, l);
    }
    closesocket(s);
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    int error = WSAStartup(wVersionRequested, &wsaData);
    if (error != 0)
    {
        return 1;
    }

    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);

    if (s == INVALID_SOCKET)
    {
        WSACleanup();
        cout << endl << "-!!!- Socket error -!!!-" << endl;
        return 1;
    }

    struct sockaddr_in local_addr;
    local_addr.sin_port = htons(1280);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = 0;
    int len = sizeof(local_addr);

    if (bind(s, (struct sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR)
    {
        closesocket(s);
        WSACleanup();
        cout << "-!!!- Binding failed -!!!-" << endl;
        return 1;
    }

    cout << "-----SERVER IS ONLINE-----" << endl;

    char buf[10];
    while (recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&local_addr, &len) > 0)
    {
        char choice = buf[0];
        switch (choice)
        {
        case '1':
        {
            Service::add(s, local_addr, len);
            break;
        }
        case '2':
        {
            Service::show(s, local_addr, len);
            break;
        }
        case '3':
        {
            Service::edit(s, local_addr, len);
            break;
        }
        case '4':
        {
            Service::deleteStudent(s, local_addr, len);
            break;
        }
        case '5':
        {
            Service::find(s, local_addr, len);
            break;
        }
        }
    }

    closesocket(s);
    WSACleanup();
    return 0;
}