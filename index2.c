
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int main()
{
    DWORD bytesNeeded = 0, nrServicii = 0, resumeHandle = 0;

    // deschidem SCM-ul
    SC_HANDLE hSCM = OpenSCManagerA(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!hSCM)
    {
        printf("Nu s-a putut deschide SCM (eroare %lu). Ruleaza ca admin\n", GetLastError());
        system("pause");
        return 1;
    }

    // primul apel ca sa aflam cat spatiu ne trebuie
    EnumServicesStatusExA(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32,
        SERVICE_ACTIVE, NULL, 0, &bytesNeeded, &nrServicii, &resumeHandle, NULL);

    BYTE* buffer = (BYTE*)malloc(bytesNeeded);

    // al doilea apel, acum cu buffer-ul alocat
    resumeHandle = 0;
    EnumServicesStatusExA(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32,
        SERVICE_ACTIVE, buffer, bytesNeeded, &bytesNeeded, &nrServicii, &resumeHandle, NULL);

    ENUM_SERVICE_STATUS_PROCESSA* srv = (ENUM_SERVICE_STATUS_PROCESSA*)buffer;

    printf("\nServicii active: %lu\n\n", nrServicii);
    printf("%-4s %-30s %-7s %s\n", "Nr", "Nume serviciu", "PID", "Descriere");

    for (DWORD i = 0; i < nrServicii; i++)
    {
        printf("%-4lu %-30s %-7lu %s\n",
            i + 1,
            srv[i].lpServiceName,
            srv[i].ServiceStatusProcess.dwProcessId,
            srv[i].lpDisplayName);
    }

    printf("\nTotal: %lu servicii\n", nrServicii);

    free(buffer);
    CloseServiceHandle(hSCM);
    system("pause");
    return 0;
}
