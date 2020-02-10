#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <numeric>

using namespace std;

DWORD WINAPI ThreadFunction(LPVOID lPtr);

vector<string> datafile;
string timer(void);

int main(int argc, char* argv[])
{
    HANDLE hThreadArray[datafile.size()];
    vector<int> order = {1, 4, 5, 6, 7, 2, 3, 0};
    string GetLastErrorStdStr();
    string str;

    HANDLE hFile;
    DWORD dwBytesWritten = 0;
    DWORD dwBytesToWrite = 0;
    BOOL bErrorFlag = FALSE;

    hFile = CreateFile("threadtime.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "System Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        return 1;
    }

    for(const int &i : order) {
        hThreadArray[i] = CreateThread(NULL, 0, ThreadFunction, 0, 0, 0);

        datafile.push_back("Watek nr ");
        datafile.push_back(to_string(order[i]));
        datafile.push_back(" zakonczono o:");
        datafile.push_back(timer());
        datafile.push_back("\r\n");
        Sleep(500);
    }

    if(hThreadArray==NULL) {
        printf("Blad tworzenia watku!\n");
    }

    WaitForMultipleObjects(datafile.size(), hThreadArray, TRUE, INFINITE);

    for (int i = 0; i<(int)datafile.size(); i++) {
        CloseHandle(hThreadArray[i]);
    }

    str = accumulate(begin(datafile), end(datafile), string(),
    [](string lhs, const string &rhs) {
        return lhs.empty() ? rhs : lhs + "" + rhs;
    });

    dwBytesToWrite = (DWORD)str.size();

    bErrorFlag = WriteFile(hFile, str.c_str(), str.size(), &dwBytesWritten, NULL);
    if (FALSE == bErrorFlag) {
        cout << "System Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        return 2;
    }
    else {
        if (dwBytesWritten != dwBytesToWrite) {
            cout << "Blad!: dwBytesWritten != dwBytesToWrite\n";
            return 3;
        }
        else {
            cout << "\nZapisano pomyslnie " << dwBytesWritten << " bajtow do pliku: threadtime.txt\n" << endl;
        }
    }

    CloseHandle(hFile);

    for (int i = 0; i<(int)datafile.size(); i++) {
        cout << datafile[i];
    }

    cout << endl;

    return 0;
}

DWORD WINAPI ThreadFunction(LPVOID lPtr)
{
    cout << "Jestem w watku " << GetCurrentThreadId() << endl;
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
