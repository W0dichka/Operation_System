#include "employee.h"
#include <iostream>
#include <process.h>
#include <windows.h>
#include <fstream>
#include <conio.h>
#include <algorithm>
#include <string>

std::ofstream fout;
std::ifstream fin;
int id;
HANDLE* finished;
employee temp;
int num_of_emp;
int num_of_acts;
bool* access;
std::string file_name;
employee* emps;
int num_of_acts;
int* access;

int emp_сmp(const void* p1, const void* p2)
{
    return ((employee*)p1)->num - ((employee*)p2)->num;
}

DWORD WINAPI messaging(LPVOID p) {
    HANDLE hPipe = (HANDLE)p;
    while (true) {
        char message[20];
        DWORD read_b;
        bool isRead = ReadFile(hPipe, message, 20, &read_b, NULL);
        if (strlen(message) > 0) {
            DWORD bytesWritten;
            std::cout << "Enter id of employee" << std::endl;
            std::cin >> id;
            temp.num = id;
            employee* sending = bsearch(&temp, emps,num_of_emp, sizeof(employee), emp_cmp);
            int ind = sending - emps;
            if (access[ind])
            {
               if (message == "1")
               {
                   std::cout << "Reading" << std::endl;
                   access[ind] = true;
               }
               else
               {
                   std::cout << "Writing " << std::endl;
               }
            }
        }
        else
        {
            std::cout << "Wrong input" << std::endl;
        }
    }
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    return 0;
}



int main() {
    std::cout << "Input name of file: ";
    std::cin >> file_name;
    std::cout << "Input count of employees: ";
    std::cin >> num_of_emp;
    emps = new employee[num_of_emp];
    std::cout << "Enter ID, name, hours " << std::endl;
    for (int i = 0; i < num_of_emp; i++) 
    {
        std::cin >> emps[i].num >> emps[i].name >> emps[i].hours;
    }
    fout.open(file_name, std::fstream::binary);
    fout.write(reinterpret_cast<const char*>(emps), sizeof(employee) * num_of_emp);
    fout.close();
    qsort(emps, num_of_emp, sizeof(employee), emp_cmp);
    std::cout << "Enter number of actions" << std::endl;
    for (int i = 0; i < num_of_emp; i++) 
    {
        access[i] = false;
    }
    HANDLE Starting = CreateEvent(NULL, TRUE, FALSE, "START_ALL");
    finished = new HANDLE[num_of_acts];
    std::string temp;
    for (int i = 0; i < num_of_acts; i++) 
    {
        std::string args += "READY_EVENT_";
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        si.cb = sizeof(STARTUPINFO);
        if (!CreateProcess(NULL, args, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) 
        {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    WaitForMultipleObjects(num_of_acts, finished, TRUE, INFINITE);
    SetEvent(Starting);
    HANDLE hPipe;
    HANDLE* hThreads = new HANDLE[num_of_acts];
    for (int i = 0; i < num_of_acts; i++) 
    { 
        hPipe = CreateNamedPipe("pipe_name".c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, NULL);
        if (!ConnectNamedPipe(hPipe, NULL)) 
        {
            std::cout << "No clients." << std::endl;
            break;
        }
        hThreads[i] = CreateThread(NULL, 0, messaging, (LPVOID)hPipe, 0, NULL);
    }
    WaitForMultipleObjects(num_of_acts, hThreads, TRUE, INFINITE);
    std::cout << "Connection finished" << std::endl;
    for (int i = 0; i < num_of_emp; i++) 
    {
        std::cout << "ID - " << emps[i].num << " Name = " << emps[i].name << " Hours = " << emps[i].hours << std::endl;
    }
}