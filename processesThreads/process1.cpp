#include <iostream>
#include <windows.h>
#include <string>
#include <time.h>
#include <vector>
#include <numeric>
#include <sstream>

using namespace std;

string GetLastErrorStdStr();
CRITICAL_SECTION critical;

struct PARAMETERS
{
    vector<double> d;
    int s = 0;
};

DWORD WINAPI SummationThread(void* param)
{
    PARAMETERS* params = (PARAMETERS*)param;
    for (int i=5; i<10; i++){
        (params->s) = (params->s) + (params->d)[i];
    }
    return 0;
}

DWORD WINAPI DisplayThread(void* param)
{
    PARAMETERS* params = (PARAMETERS*)param;
    cout << "Suma liczb = " << (params->s) << endl;
    return 0;
}

int main(int argc, char* argv[])
{
    // Polskie znaki w GetLastErrorStdStr()
    setlocale(LC_ALL, "polish");

    HANDLE hFile;
    DWORD dwBytesRead = 0;
    vector<int> vecIntNumbers;

    // Uruchomienie procesu 2
    STARTUPINFOW si = { 0 };
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = { 0 };

    CreateProcessW(L"D:\\Programming\\SO1\\zal3\\process2\\bin\\Debug\\process2.exe",
                   NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    Sleep(1000);

    //Spinlock
    do {
        cout << "Oczekiwanie na plik w spinlocku...";
        hFile=CreateFile(TEXT("D:\\Programming\\SO1\\zal3\\process2\\numbers.txt"),
                         GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    } while(hFile == INVALID_HANDLE_VALUE);

    DWORD fileSize = GetFileSize(hFile, NULL);
    int bufferSize = fileSize/sizeof(char);
    char readBuffer[bufferSize-1];
    // Odczyt jednego znaku mniej niz rozmiar bufora, aby zaoszczedzic miejsce na konczacy znak NULL.

    if(FALSE == ReadFile(hFile, &readBuffer[0], fileSize, &dwBytesRead, NULL)) {
        cout << "\n\nSystem Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        CloseHandle(hFile);
        return 1;
    } else
        cout << "\nPlik otwarty do odczytu(p1)!" << endl;

    if (dwBytesRead > 0) {
        // NULL character
        readBuffer[dwBytesRead+1] = '\0';
        cout << "Odczytano " << dwBytesRead << " bajtow z pliku: numbers.txt" << endl;
        cout << "\n" << readBuffer << endl;
    } else {
        cout << "Nie odczytano zadnych danych z pliku!" << endl;
    }

    CloseHandle(hFile);

    double num;
    vector<double> vecDnumbers;
    stringstream lineStream(readBuffer);

    while (lineStream >> num) vecDnumbers.push_back(num);

    PARAMETERS params;
    params.d = vecDnumbers;

    InitializeCriticalSection(&critical);

    HANDLE thdHandle1 = CreateThread(NULL, 0, SummationThread, &params, 0, NULL);
    WaitForSingleObject(thdHandle1, INFINITE);
    CloseHandle(thdHandle1);

    HANDLE thdHandle2 = CreateThread(NULL, 0, DisplayThread, &params, 0, NULL);
    WaitForSingleObject(thdHandle2, INFINITE);
    CloseHandle(thdHandle2);

    DeleteCriticalSection(&critical);

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
