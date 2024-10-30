#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h> // For toolhelp functions
#include <string.h>

#define UBISOFT_PROCESS_NAME "UplayWebCore.exe"
#define UBISOFT_PROCESS_NAME2 "upc.exe"
#define RAINBOW_SIX_PROCESS_NAME "RainbowSix_DX11.exe"

// Function to set the priority of a process by its ID
int SetProcessPriority(DWORD processID, DWORD priorityClass) {
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, processID);
    if (hProcess == NULL) {
        printf("Could not open process %lu for setting priority.\n", processID);
        return 0;
    }

    int result = SetPriorityClass(hProcess, priorityClass);
    if (!result) {
        printf("Failed to set priority class for process %lu.\n", processID);
    }
    
    CloseHandle(hProcess);
    return result;
}

// Function to find and modify the priority of a process by name
void FindAndSetPriorityByName(const char* processName, DWORD priorityClass) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Create snapshot of processes
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Failed to create snapshot of processes.\n");
        return;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Iterate through the processes
    if (Process32First(hSnapshot, &pe)) {
        do {
            // Compare the process name with the target name (case insensitive)
            if (stricmp(pe.szExeFile, processName) == 0) {
                printf("Found process %s with ID %lu.\n", processName, pe.th32ProcessID);
                if (SetProcessPriority(pe.th32ProcessID, priorityClass)) {
                    printf("Set process %s (ID %lu) to priority level %d.\n", processName, pe.th32ProcessID, priorityClass);
                } else {
                    printf("Failed to set priority for process %s (ID %lu).\n", processName, pe.th32ProcessID);
                }
            }
        } while (Process32Next(hSnapshot, &pe)); // Move to the next process
    }

    CloseHandle(hSnapshot);
}

int main() {
    printf("Searching for processes...\n");

    // Set low priority for all Ubisoft Connect processes
    FindAndSetPriorityByName(UBISOFT_PROCESS_NAME, IDLE_PRIORITY_CLASS);
    FindAndSetPriorityByName(UBISOFT_PROCESS_NAME2, IDLE_PRIORITY_CLASS);

    // Set high priority for Rainbow Six process
    FindAndSetPriorityByName(RAINBOW_SIX_PROCESS_NAME, HIGH_PRIORITY_CLASS);

    printf("Process priority modification completed.\n");
    printf("Press Enter to exit.\n");
    getchar();
    return 0;
}
