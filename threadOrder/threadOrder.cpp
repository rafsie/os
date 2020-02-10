#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <process.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <ctime>
#include <sstream>
#define MAX_THREADS 8

using namespace std;

vector<string> datafile;
string GetLastErrorStdStr();
string timer(void);

DWORD WINAPI ThreadFunction( LPVOID lpParam );

typedef struct MyData {
    vector<int> vecIntNumbers;
} MYDATA, *PMYDATA;

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "polish");

    PMYDATA pDataArray[MAX_THREADS];
    DWORD   dwThreadIdArray[MAX_THREADS];
    HANDLE  hThreadArray[MAX_THREADS];

    for(int i=0; i<MAX_THREADS; i++) {
        pDataArray[i] = new MYDATA;

        if( pDataArray[i] == NULL ) {
            ExitProcess(2);
        }
    }

    vector<int> order = {1, 4, 5, 6, 7, 2, 3, 0};

    // Create threads
    for(const int &i : order) {
        hThreadArray[i] = CreateThread(NULL, 0, ThreadFunction, 0, 0, &dwThreadIdArray[i]);
        //cout << "Watek nr: " << order[i] << " ";
        datafile.push_back("\nWatek nr ");
        datafile.push_back(to_string(order[i]));
        datafile.push_back(" zakonczono o:");
        datafile.push_back(timer());
        Sleep(1000);
    }
    // Check the return value for success.
    // If CreateThread fails, terminate execution.
    // This will automatically clean up threads and memory.
    for(int i=0; i<MAX_THREADS; i++) {
        if (hThreadArray[i] == NULL) {
            ExitProcess(3);
        }
    }

    // Wait until all threads have terminated.
    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    // Close all thread handles and free memory allocations.
    for (int i = 0; i<MAX_THREADS; i++) {
        CloseHandle(hThreadArray[i]);
        if (pDataArray[i] != NULL) {
            delete pDataArray[i];
            pDataArray[i] = NULL;  // Ensure address is not reused.
        }
    }

    for (int i = 0; i<(int)datafile.size(); i++) {
        cout << datafile[i];
    }

    return 0;
}

string GetLastErrorStdStr()
{
    DWORD error = GetLastError();
    if (error) {
        LPVOID lpMsgBuf;
        DWORD bufLen = FormatMessage(
                           FORMAT_MESSAGE_ALLOCATE_BUFFER |
                           FORMAT_MESSAGE_FROM_SYSTEM |
                           FORMAT_MESSAGE_IGNORE_INSERTS,
                           NULL,
                           error,
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (LPTSTR) &lpMsgBuf,
                           0, NULL );
        if (bufLen) {
            LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
            string result(lpMsgStr, lpMsgStr+bufLen);

            LocalFree(lpMsgBuf);

            return result;
        }
    }
    return string();
}

string timer(void)
{
    auto t = time(nullptr);
    auto tm = *localtime(&t);

    ostringstream oss;
    oss << put_time(&tm, " %H:%M:%S");
    auto str = oss.str();
    return str;
}

DWORD WINAPI ThreadFunction( LPVOID lpParam )
{
    cout << "Jestem w watku " << GetCurrentThreadId() << endl;
    return 0;
}
