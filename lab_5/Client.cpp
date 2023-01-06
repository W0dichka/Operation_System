#include <sstream>
#include "employee.h"
#include <iostream>
#include <process.h>
#include <windows.h>
#include <fstream>
#include <conio.h>
#include <algorithm>
#include <string>

int main(int argc, char** argv) {
    HANDLE handleReadyEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, argv[1]);
    HANDLE handleStartEvent = OpenEvent(SYNCHRONIZE, FALSE, ipName.c_str());
    SetEvent(handleReadyEvent);
    WaitForSingleObject(handleStartEvent, INFINITE);
    HANDLE handlePipe;
    while (true) 
    {
        handlePipe = CreateFile(pipeName.c_str(), GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_READ, NULL,
            OPEN_EXISTING, 0, NULL);
        if (INVALID_HANDLE_VALUE != handlePipe) 
        {
            break;
        }
    }
    while (true) {
        std::cout << "Input 1 to read or 2 to write" << std::endl;
        std::string action_com;
        std::cin >> action_com;
        int size = action_com.size();
        bool isSent;
        DWORD bytesWritten;
        isSent = WriteFile(handlePipe, (LPWSTR)(action_com).c_str(), size,
            &bytesWritten, NULL);
        DWORD readBytes;
        employee tempEmployee;
        bool isRead = ReadFile(handlePipe, &tempEmployee,
            sizeof(tempEmployee), &readBytes, NULL);
        tempEmployee.print();
        if ('2' == action_com[0]) {
            std::cout << "Enter ID, name, hours";
            std::cin >> tempEmployee.num >> tempEmployee.name >> tempEmployee.hours;
            isSent = WriteFile(handlePipe, &tempEmployee,
                sizeof(tempEmployee), &bytesWritten, NULL);
        }
    }
    return 0;
}
