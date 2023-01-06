#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>

std::ofstream fout;
HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"mutex");
HANDLE write = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"write");
HANDLE read = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"read");

int main(int argc, char* argv[])
{
	HANDLE ready = OpenEvent(EVENT_MODIFY_STATE, FALSE, (to_wstring(atoi(argv[3])) + L"ready").c_str());
	while (true)
	{
		int chek;
		std::cout << "Stop = 0 , Continue = 1" << std::endl;
		std::cin >> chek;
		if (chek == 0)
		{
			break;
		}
		std::cout << "Enter message" << std::endl;
		std::string message;
		std::cin >> message;
		message += '\n';
		WaitForSingleObject(write, INFINITE);
		WaitForSingleObject(mutex, INFINITE);
		fout.write((const char*)&message, sizeof(std::string));
		ReleaseMutex(mutex);
		ReleaseSemaphore(read, 1, NULL);
	}
	fout.close();
	CloseHandle(mutex);
	CloseHandle(ready);
	CloseHandle(write);
	CloseHandle(read);
}