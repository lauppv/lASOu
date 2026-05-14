#include <windows.h>
#include <stdio.h>

int main() {
    char szCaleRelativa[MAX_PATH];
    HANDLE hFisier;
    DWORD dwBytesRead;
    BYTE bBuffer[4096];

    printf("Introduceti calea relativa a fisierului: ");
    if (fgets(szCaleRelativa, MAX_PATH, stdin) == NULL) {
        printf("Eroare la citirea caii.\n");
        return 1;
    }

    // eliminam newline-ul de la sfarsit
    size_t nLen = strlen(szCaleRelativa);
    if (nLen > 0 && szCaleRelativa[nLen - 1] == '\n')
        szCaleRelativa[nLen - 1] = '\0';

    hFisier = CreateFileA(
        szCaleRelativa,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFisier == INVALID_HANDLE_VALUE) {
        printf("Eroare: fisierul \"%s\" nu exista sau nu poate fi deschis (cod %lu).\n",
               szCaleRelativa, GetLastError());
        return 1;
    }

    while (ReadFile(hFisier, bBuffer, sizeof(bBuffer) - 1, &dwBytesRead, NULL) && dwBytesRead > 0) {
        bBuffer[dwBytesRead] = '\0';
        printf("%s", (char*)bBuffer);
    }

    CloseHandle(hFisier);
    return 0;
}
