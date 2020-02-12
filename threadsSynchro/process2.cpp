#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

string GetLastErrorStdStr();
double GetAverage(vector<int> vecIntNumbers);
int GetMin(vector<int> vecIntNumbers);
int GetMax(vector<int> vecIntNumbers);

int main(int argc, char *argv[])
{
    // Polskie znaki w GetLastErrorStdStr()
    setlocale(LC_ALL, "polish");
    vector<string> vresultscd;
    string strresultscd;

    HANDLE hFile;
    HANDLE rFile;
    DWORD dwBytesRead = 0;
    DWORD dwBytesWritten = 0;
    DWORD dwBytesToWrite = 0;

    BOOL rErrorFlag = FALSE;

    // Spinlock
    do {
        cout << "Oczekiwanie na plik w spinlocku...";
   
        hFile=CreateFile(TEXT("D:\\threadsSynchro\\process1\\random.txt"),
                         GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    while(hFile == INVALID_HANDLE_VALUE); //&& time < 15);

    DWORD fileSize = GetFileSize(hFile, NULL);
    int bufferSize = fileSize/sizeof(char);
    char readBuffer[bufferSize-1];

    if(FALSE == ReadFile(hFile, &readBuffer[0], fileSize, &dwBytesRead, NULL)) {
        cout << "\n\nSystem Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        CloseHandle(hFile);
        return 1;
    }
    else
        cout << "\nPlik otwarty do odczytu!" << endl;

    if (dwBytesRead > 0) {
        // NULL character
        readBuffer[dwBytesRead+1] = '\0';
        cout << "Odczytano " << dwBytesRead << " bajtow z pliku: random.txt" << endl;
        cout << "\n" << readBuffer << endl;
    }
    else {
        cout << "Nie odczytano zadnych danych z pliku!" << endl;
    }

    CloseHandle(hFile);

    istringstream ss(readBuffer);
    string numbers;
    vector<string> vecStrNumbers;

    while(getline(ss, numbers, ' ')) {
        vecStrNumbers.push_back(numbers);
    }

    cout << endl;

    vector<int> vecIntNumbers;
    int size_v = vecStrNumbers.size();
    for(int i=0; i<size_v; i++) {
        vecIntNumbers.push_back(atoi((vecStrNumbers[i]).c_str()));
        //cout << vecIntNumbers.at(i) << " ";
    }

    cout << "Srednia wylosowanych liczb wynosi: " << GetAverage(vecIntNumbers) << endl;
    cout << "Wartosc minimalna: " << GetMin(vecIntNumbers) << endl;
    cout << "Wartosc maksymalna: " << GetMax(vecIntNumbers) << endl;
    cout << endl;

    vresultscd.push_back(to_string(GetAverage(vecIntNumbers)));
    vresultscd.push_back("\t\t");
    vresultscd.push_back(to_string(GetMin(vecIntNumbers)));
    vresultscd.push_back("\t\t");
    vresultscd.push_back(to_string(GetMax(vecIntNumbers)));
    vresultscd.push_back("\r\n");

    strresultscd = accumulate(begin(vresultscd), end(vresultscd), string(),
    [](string lhs, const string &rhs) {
        return lhs.empty() ? rhs : lhs + "" + rhs;
    });

    dwBytesToWrite = (DWORD)strresultscd.size();

    rFile=CreateFile("D:\\threadsSynchro\\process1\\results.txt",
                         FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (rFile == INVALID_HANDLE_VALUE) {
        cout << "System Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        return 2;
    }

    rErrorFlag = WriteFile(rFile, strresultscd.c_str(), strresultscd.size(), &dwBytesWritten, NULL);
    if (FALSE == rErrorFlag) {
        cout << "System Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        return 3;
    }
    else {
        if (dwBytesWritten != dwBytesToWrite) {
            cout << "Blad!: dwBytesWritten != dwBytesToWrite\n";
            return 4;
        }
    }

    CloseHandle(rFile);

    return 0;
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

double GetAverage(vector<int> vecIntNumbers)
{
    double sum = 0;
    for(int i=0; i<(int)vecIntNumbers.size(); ++i) {
        sum += vecIntNumbers[i];
    }
    return sum/vecIntNumbers.size();
}

int GetMin(vector<int> vecIntNumbers)
{
    sort(vecIntNumbers.begin(), vecIntNumbers.end());
    return vecIntNumbers.front();
}

int GetMax(vector<int> vecIntNumbers)
{
    sort(vecIntNumbers.begin(), vecIntNumbers.end());
    return vecIntNumbers.back();
}
