#include <iostream>
#include <windows.h>
#include <string>
#include <time.h>
#include <vector>
#include <numeric>

#define MIN 1
#define MAX 100

using namespace std;

int random(int min, int max);
string GetLastErrorStdStr();

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "polish");

    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );

    if(argc != 4) {
        cout << "Musisz podac trzy parametry: \nilosc losowanych liczb, \nodstep w sekundach \ni ilosc powtorzen!" << endl;
        return 1;
    }

    int il_liczb = stoi(argv[1]);
    int odstep_sek = stoi(argv[2]);
    int il_powt = stoi(argv[3]);
    vector<string> temp;
    vector<string> vresults;
    string strresults;
    string str;

    HANDLE hFile;
    HANDLE rFile;
    DWORD dwBytesWritten = 0;
    DWORD dwBytesToWrite = 0;
    BOOL bErrorFlag = FALSE;
    BOOL rErrorFlag = FALSE;

    vresults.push_back("Total numbers: ");
    vresults.push_back(to_string(il_liczb));
    vresults.push_back(" | ");
    vresults.push_back("Version: Singlethread");
    vresults.push_back(" | ");
    vresults.push_back("CPU: ");
    vresults.push_back(to_string(sysinfo.dwNumberOfProcessors));
    vresults.push_back(" | ");
    vresults.push_back("Replies: ");
    vresults.push_back(to_string(il_powt));
    vresults.push_back(" | ");
    vresults.push_back("Pause: ");
    vresults.push_back(to_string(odstep_sek));
    vresults.push_back("\r\n");
    vresults.push_back("\r\n");
    vresults.push_back("Avg: ");
    vresults.push_back("\t\t\t");
    vresults.push_back("Min: ");
    vresults.push_back("\t\t");
    vresults.push_back("Max: ");
    vresults.push_back("\r\n");
    vresults.push_back("\r\n");

    strresults = accumulate(begin(vresults), end(vresults), string(),
    [](string lhs, const string &rhs) {
        return lhs.empty() ? rhs : lhs + "" + rhs;
    });

    dwBytesToWrite = (DWORD)strresults.size();

    rFile = CreateFile("results.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (rFile == INVALID_HANDLE_VALUE) {
        cout << "System Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        return 5;
    }

    rErrorFlag = WriteFile(rFile, strresults.c_str(), strresults.size(), &dwBytesWritten, NULL);
    if (FALSE == rErrorFlag) {
        cout << "System Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        return 6;
    }
    else {
        if (dwBytesWritten != dwBytesToWrite) {
            cout << "Blad!: dwBytesWritten != dwBytesToWrite\n";
            return 7;
        }
    }

    CloseHandle(rFile);

    for(int i=0; i<il_powt; i++){

    hFile = CreateFile("random.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "System Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        return 2;
    }

    // Uruchomienie procesu 2
    STARTUPINFOW si = { 0 };
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = { 0 };

    BOOL success = CreateProcessW(
                       L"D:\\threadsSynchro\\process2\\bin\\Debug\\process2.exe",
                       NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    for (int i=0; i<il_liczb; i++) {
        temp.push_back(to_string(random(MIN, MAX)));
    }

    str = accumulate(begin(temp), end(temp), string(),
    [](string lhs, const string &rhs) {
        return lhs.empty() ? rhs : lhs + " " + rhs;
    });

    dwBytesToWrite = (DWORD)str.size();

    bErrorFlag = WriteFile(hFile, str.c_str(), str.size(), &dwBytesWritten, NULL);
    if (FALSE == bErrorFlag) {
        cout << "System Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        return 3;
    }
    else {
        if (dwBytesWritten != dwBytesToWrite) {
            cout << "Blad!: dwBytesWritten != dwBytesToWrite\n";
            return 4;
        }
        else {
            cout << "Zapisano pomyslnie " << dwBytesWritten << " bajtow do pliku: random.txt" << endl;
        }
    }

    CloseHandle(hFile);

    if (success) {
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    Sleep(1000*odstep_sek);
    temp.clear();
    }
    return 0;
}

// Funkcja zwraca liczby losowe z zadanego przedzialu
int random(int min, int max)
{
    static bool first = true;
    if (first) {
        srand(time(NULL));
        first = false;
    }
    return min + rand() % ((max + 1) - min);
}

// Funkcja pobiera ostatni kod bledu, jesli taki istnieje
// i pobiera powiazana z nim wiadomosc tekstowa, ktora jest
// nastepnie konwertowana na standardowy ciag znakow i zwracana.
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
