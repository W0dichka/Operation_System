#include <iostream>
#include <thread>
#include "windows.h"
#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;
int sleep = 5;
mutex globalMutex;
mutex mainMutex;
mutex* localMutex;
condition_variable* cvContinue;
condition_variable* cvFinish;
condition_variable* cvWait;
int index_kill;

bool work_thread(vector<bool>& check) {
    for (bool i : check)
    {
        if (i)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void marker(int thread_ind, vector<int>& array)
{
    vector<int> marked;
    while (true)
    {
        int rand_n = rand() % array.size();
        if (array[rand_n] == 0)
        {
            marked.push_back(rand_n);
            Sleep(sleep);
            array[rand_n] = thread_ind;
            Sleep(sleep);
        }
        else
        {
            globalMutex.unlock();
            if (cvFinish[thread_ind - 1].wait_for(uniqueLock, std::chrono::milliseconds(250)) ==
                cv_status::timeout) {
                for (int markedIndex : marked) {
                    array[markedIndex] = 0;
                }
                return;
            }
            else
                globalMutex.lock();
        }
    }
}

int main()
{
    cout << "Enter size of array" << endl;
    int n;
    cin >> n;
    cout << "Entre size of threds" << endl;
    int thread_n;
    cin >> thread_n;
    unique_lock uniqueLock(mainMutex);
    globalMutex.lock();
    vector<int> array(n,0);
    vector<thread> threads;
    localMutex = new mutex[thread_n];
    cvContinue = new condition_variable[thread_n];
    cvFinish = new condition_variable[thread_n];
    cvWait = new condition_variable[thread_n];
    globalMutex.unlock();

    for (int i = 0; i < thread_n; i++)
    {
            globalMutex.unlock();
            threads.emplace_back(marker, i + 1, ref(array));
            cvWait[i].wait_for(uniqueLock, std::chrono::seconds(250));
            globalMutex.lock();
    }

    vector<bool> checking(thread_n, true);
    while(work_thread(checking))
    {
        globalMutex.unlock();
        cout << "Enter index" << endl;
        cin >> index_kill;
        cvContinue[index_kill].notify_all();
        threads[index_kill].join();
        checking[index_kill] = false;

        for (int i = 0; i < thread_n; i++)
        {
            if (checking[i] == true)
            {
                globalMutex.unlock();
                cvContinue[i].notify_all();
                Sleep(sleep);
                cvFinish[i].notify_all();
                cvWait[i].wait_for(uniqueLock, chrono::seconds(250));
                globalMutex.lock();
            }
        }
        for (int i = 0; i < n; i++) 
        {
            cout << array[i] << " " ;
        }
        cout << endl;
    }
    for (int i = 0; i < array.size(); i++)
    {
        cout << array[i] << " ";
    }
    cout << endl;
}   