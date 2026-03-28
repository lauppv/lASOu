#include <windows.h>
#include <stdio.h>

void AfiseazaSubchei(HKEY hKey) {
    DWORD index = 0;
    WCHAR subKeyName[256];
    DWORD nameSize;

    while (1) {
        nameSize = sizeof(subKeyName) / sizeof(WCHAR);

        LONG result = RegEnumKeyExW(
            hKey,
            index,
            subKeyName,
            &nameSize,
            NULL,
            NULL,
            NULL,
            NULL
        );

        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }

        if (result != ERROR_SUCCESS) {
            printf("Eroare la enumerarea subcheilor.\n");
            return;
        }

        wprintf(L"Subcheie %lu: %ls\n", index, subKeyName);
        index++;
    }
}

int main() {
    HKEY hKey;
    LONG status;

    status = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software",
        0,
        KEY_READ,
        &hKey
    );

    if (status != ERROR_SUCCESS) {
        printf("Nu s-a putut deschide cheia din registry.\n");
        return 1;
    }

    printf("Subcheile din HKEY_CURRENT_USER\\Software:\n\n");

    AfiseazaSubchei(hKey);

    RegCloseKey(hKey);
    return 0;
}
