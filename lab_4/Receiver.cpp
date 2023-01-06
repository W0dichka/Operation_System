#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>

std::ofstream fout;
std::ifstream fin;

int main()
{
	int num_of_rec;
	int num_of_send;
	std::string file_name;
	std::cout << "Enter name of file" << std::endl;
	std::cin >> file_name;
	std::cout << "Enter number of records" << std::endl;
	std::cout << "Enter number of senders" << std::endl;
	std::cin >> num_of_send;
	fout.open(file_name, std::fstream::binary);
	if (!fout.is_open()) {
		std::cout << "Error with creating file";
	}
	fout.close();
	HANDLE* senders = new HANDLE[num_of_send];
	HANDLE* events = new HANDLE[num_of_send];
	HANDLE write = CreateSemaphore(NULL, num_of_rec, num_of_rec, L"write");
	HANDLE read = CreateSemaphore(NULL, 0, num_of_rec, L"read");
	HANDLE mutex = CreateMutex(NULL, FALSE, L"mutex");
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	for (int i = 0; i < num_of_send; i++)
	{
		std::string arg = "Sender.exe";
		arg = arg + " " + file_name;
		HANDLE event = CreateEvent(NULL, FALSE, FALSE, (LPWSTR)(arg).c_str());
		events[i] = event;

		if (!CreateProcess(NULL, (LPWSTR)arg.c_str(), NULL, NULL,
			FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
			return NULL;
		}

		CloseHandle(pi.hThread);
		senders[i] =  pi.hProcess;
	}

	WaitForMultipleObjects(num_of_send, events, TRUE, INFINITE);

	while (WaitForMultipleObjects(num_of_send, senders, TRUE, 0) == WAIT_TIMEOUT)
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
		fin.open(file_name, std::fstream::binary);
		WaitForSingleObject(read, INFINITE);
		WaitForSingleObject(mutex, INFINITE);
		while (!fin.eof())
		{
			fin.read((char*)&message, sizeof(std::string));
			std::cout << message << std::endl;
		}
		ReleaseMutex(mutex);
		ReleaseSemaphore(write, 1, NULL);
		fin.close();
	}
	for (int i = 0; i < num_of_send; i++)
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		CloseHandle(events[i]);
	}
	CloseHandle(mutex);
	CloseHandle(read);
	CloseHandle(write);
}