#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>

using namespace std;

string doubleToString(double);
string GetLastErrorStdStr();

int main(int argc, char* argv[])
{
    // Polskie znaki w GetLastErrorStdStr()
    setlocale(LC_ALL, "polish");

    HANDLE hFile;
    DWORD dwBytesRead = 0;

    hFile=CreateFile(TEXT("D:\\processesThreads\\process2\\numbers.txt"),
                     GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD fileSize = GetFileSize(hFile, NULL);
    int bufferSize = fileSize/sizeof(char);
    char readBuffer[bufferSize-1];
    // Odczyt jednego znaku mniej niz rozmiar bufora, aby zaoszczedzic miejsce na konczacy znak NULL.

    if(FALSE == ReadFile(hFile, &readBuffer[0], fileSize, &dwBytesRead, NULL)) {
        cout << "\n\nSystem Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        CloseHandle(hFile);
        return 1;
    }
    else
        cout << "Plik otwarty do odczytu!(p2)" << endl;

    if (dwBytesRead > 0) {
        // NULL character
        readBuffer[dwBytesRead+1] = '\0';
        cout << "Odczytano " << dwBytesRead << " bajtow z pliku: numbers.txt" << endl;
        cout << "\n" << readBuffer << endl;
    }
    else {
        cout << "Nie odczytano zadnych danych z pliku!" << endl;
    }

    CloseHandle(hFile);

    double num;
    string str;
    stringstream lineStream(readBuffer);
    while (lineStream >> num) str.append((doubleToString(num*10)) + '\t');
    str.append("\r\n");

    DWORD dwBytesWritten = 0;
    DWORD dwBytesToWrite = 0;

    hFile = CreateFile(TEXT("D:\\processesThreads\\process2\\numbers.txt"),
                       FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "System Error Code " << "(" << GetLastError() << "): " << GetLastErrorStdStr();
        return 2;
    }

    dwBytesToWrite = (DWORD)str.size();
    BOOL bErrorFlag = FALSE;

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
            cout << "\nZapisano pomyslnie " << dwBytesWritten << " bajtow do pliku: numbers.txt" << endl;
        }
    }
    return 0;
}

string doubleToString(double d) {
    ostringstream strs;
	strs << d;
	string str = strs.str();
    return str;
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
