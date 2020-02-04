#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <chrono>
#include <tchar.h>
#include <process.h>
#include <winbase.h>
#include <string.h>
#include <algorithm>
#include <time.h>
#include <sstream>

using namespace std;

void killPID(int pidK);

int main(int argc, char *argv[])
{
    vector<string> procVect;
    string vecTostr;
    int freq = 0;
    int pidI = GetCurrentProcessId();

    HANDLE hFile1;
    DWORD bytesWritten = 0;
    PROCESSENTRY32 proc32;
    HANDLE hSnapshot1;

    time_t actualTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
    procVect.push_back(ctime(&actualTime));

    hSnapshot1 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    proc32.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(hSnapshot1, &proc32)) {
        while(Process32Next(hSnapshot1, &proc32)) {
            procVect.push_back(proc32.szExeFile);
        }
    }

    vecTostr = accumulate(begin(procVect), end(procVect), string(),
    [](string lhs, const string &rhs) {
        return lhs.empty() ? rhs : lhs + ", " + rhs;
    });

    cout << vecTostr << endl;

    hFile1 = CreateFile("D:\\processTotext\\processes.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    WriteFile(hFile1, vecTostr.c_str(), vecTostr.length(), &bytesWritten, NULL);

    CloseHandle(hSnapshot1);
    CloseHandle(hFile1);

    do {
        hSnapshot1 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        proc32.dwSize = sizeof(PROCESSENTRY32);

        time_t timeInSec;
        time(&timeInSec);

        procVect.clear();

        if(Process32First(hSnapshot1, &proc32)) {
            while(Process32Next(hSnapshot1, &proc32)) {
                procVect.push_back(proc32.szExeFile);
            }
        }

        vecTostr = accumulate(begin(procVect), end(procVect), string(),
        [](string lhs, const string &rhs) {
            return lhs.empty() ? rhs : lhs + ", " + rhs;
        });

        cout << vecTostr << endl;
        Sleep(1000);

        string key = "processTotext.exe";
        int freq = count(procVect.begin(), procVect.end(), key);

        cout << "\nCzas: " << ((long long)timeInSec) << endl;
        cout << "Aktywnych procesow: " << freq << endl;
        cout << "PID: " << pidI << "\n" << endl;

        vector<DWORD> procPid;

        if(freq >= 1) {
            wstring targetProcessName = L"processTotext.exe";

            HANDLE hSnapshot2 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

            PROCESSENTRY32W entry;
            entry.dwSize = sizeof entry;

            if (!Process32FirstW(hSnapshot2, &entry)) {
                return 0;
            }

            do {
                if (wstring(entry.szExeFile) == targetProcessName) {
                    procPid.emplace_back(entry.th32ProcessID);
                }

            } while (Process32NextW(hSnapshot2, &entry));
        }

        if(freq >= 2) {
            for (int i=0; i<int(procPid.size()-1); ++i) {
                killPID(procPid[i]);
            }
        }

    } while(freq < 1);

    return 0;
}

void killPID(int pidK)
{
    HANDLE hProc;
    hProc = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, TRUE, pidK);
    TerminateProcess(hProc, 0);
}
