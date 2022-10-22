#include <windows.h>
#include <iostream>

using namespace std;

int avg = 0;
int* mas;
int n;
int mini = 0;
int maxi = 0;

DWORD WINAPI Avg()
{
	for (int i = 0; i < n; i++)
	{
		avg += mas[i];
		Sleep(7);
	}
	avg /= n;
	cout << "Average = " << avg << endl;
	return 0;
}

DWORD WINAPI Max_Min()
{
	int maximum = mas[0];
	int minimum = mas[0];
	for (int i = 0; i < n; i++)
	{
		if (maximum < mas[i]) {
			maxi = mas[i];
			maxi = i;
		}
		Sleep(7);
		if (minimum > mas[i]) {
			minimum = mas[i];
			mini = i;
		}
		Sleep(7);
	}
	cout << "Max = " << maxi << endl;
	cout << "Min = " << mini << endl;
	return 0;
}

DWORD WINAPI Main()
{
	HANDLE avgThread;
	HANDLE maxiThread;
	DWORD MaxThread;
	DWORD AvgThread;
	cout << "Enter size of array" << endl;
	cin >> n;
	mas = new int[n];
	cout << "Enter array" << endl;
	for (int i = 0; i < n; i++)
	{
		cin >> mas[i];
	}
	avgThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Avg, 0, 0, &AvgThread);
	maxiThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Max_Min, 0, 0, &MaxThread);
	WaitForSingleObject(avgThread, INFINITE);
	WaitForSingleObject(maxiThread, INFINITE);
	CloseHandle(avgThread);
	CloseHandle(maxiThread);
	mas[mini] = avg;
	mas[maxi] = avg;
	for (int i = 0; i < n; i++)
	{
		cout << mas[i] << " ";
	}
	return 0;
}

int main()
{
	HANDLE hThread;
	DWORD IDThread;
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Main, 0, 0, &IDThread);
	if (hThread == NULL)
		return GetLastError();
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	return 0;
}