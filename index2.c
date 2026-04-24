#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>

// Functie pentru Tema 2: cautam DLL-urile incarcate de un proces anume
void AfiseazaDLLuriProces(DWORD processID) {
    if (processID == 0) return; // sarim peste procesul "Idle"

    // facem o poza (snapshot) la tot ce misca in procesul asta la capitolul module
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);

    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        // Unele procese de sistem sunt "incuiate" chiar si pentru admin
        printf("      [!] Nu pot accesa modulele (Eroare sau proces protejat)\n");
        return;
    }

    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    // incepem sa listam modulele gasite in snapshot
    if (!Module32First(hModuleSnap, &me32)) {
        CloseHandle(hModuleSnap);
        return;
    }

    printf("      [DLL-uri gasite]:\n");
    int i = 0;
    do {
        // %ls pentru ca szModule vine ca Wide Char (Unicode)
        wprintf(L"        %d. %s\n", ++i, me32.szModule);
    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
}

int main() {
    DWORD bytesNeeded = 0, nrServicii = 0, resumeHandle = 0;

    // deschidem SCM-ul (Service Control Manager)
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!hSCM) {
        printf("Nu s-a putut deschide SCM (eroare %lu). Ruleaza ca admin!\n", GetLastError());
        system("pause");
        return 1;
    }

    // primul apel ca sa vedem cat spatiu trebuie sa alocam in buffer
    EnumServicesStatusEx(
        hSCM,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_ACTIVE,
        NULL,
        0,
        &bytesNeeded,
        &nrServicii,
        &resumeHandle,
        NULL);

    BYTE* buffer = (BYTE*)malloc(bytesNeeded);
    if (buffer == NULL) {
        printf("N-am putut aloca memorie.\n");
        return 1;
    }

    // al doilea apel, acum avem buffer-ul gata pregatit
    resumeHandle = 0;
    if (!EnumServicesStatusEx(
        hSCM,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_ACTIVE,
        buffer,
        bytesNeeded,
        &bytesNeeded,
        &nrServicii,
        &resumeHandle,
        NULL)) {
        printf("Eroare la citirea serviciilor (cod %lu).\n", GetLastError());
        free(buffer);
        CloseServiceHandle(hSCM);
        return 1;
    }

    ENUM_SERVICE_STATUS_PROCESS* srv = (ENUM_SERVICE_STATUS_PROCESS*)buffer;

    printf("Am gasit %lu servicii active.\n", nrServicii);
    printf("===============================================================================\n");

    for (DWORD i = 0; i < nrServicii; i++) {
        printf("\n[%lu] Nume: %s\n", i + 1, srv[i].lpServiceName);
        printf("    Descriere: %s\n", srv[i].lpDisplayName);
        printf("    PID:       %lu\n", srv[i].ServiceStatusProcess.dwProcessId);

        // Tema 2: Daca avem un PID valid, cautam si DLL-urile procesului
        if (srv[i].ServiceStatusProcess.dwProcessId > 0) {
            AfiseazaDLLuriProces(srv[i].ServiceStatusProcess.dwProcessId);
        }
        else {
            printf("      [!] PID invalid (probabil proces partajat)\n");
        }
        printf("-------------------------------------------------------------------------------\n");
    }

    // curatam memoria si inchidem handle-ul
    free(buffer);
    CloseServiceHandle(hSCM);

    printf("\nGata! Am listat %lu servicii si DLL-urile lor.\n", nrServicii);
    system("pause");
    return 0;
}
