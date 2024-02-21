#pragma once
#include <iostream>
#include <string>
#include <stdio.h>
#include <Windows.h>
#include <algorithm>
#include <TlHelp32.h>
#include <tchar.h>
#include <psapi.h>
#include <vector>
#include <dbghelp.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <filesystem>

namespace fs = std::filesystem;
//m_UWorld = reinterpret_cast<UWorld**>(BaseAddress + 0x926DA38);
void* mainfun;
typedef int (*mainfunType)(int);
typedef int (*FunctionPtr)(int, int);

//Injectors exe
int listdllfunc()
{
    // Load the DLL
    HMODULE hDLL = LoadLibrary(TEXT("YourDLLName.dll"));
    if (hDLL == NULL) {
        std::cerr << "Failed to load DLL\n";
        return 1;
    }

    // Get the address of the function from the DLL
    FunctionPtr func = reinterpret_cast<FunctionPtr>(GetProcAddress(hDLL, "YourFunctionName"));
    if (func == NULL) {
        std::cerr << "Failed to get function address\n";
        return 1;
    }

    // Call the function
    int result = func(2, 3);
    std::cout << "Result: " << result << std::endl;

    // Unload the DLL
    FreeLibrary(hDLL);

    return 0;
}

DWORD GetProcessIdByName(const std::wstring& processName)
{
    // Take a snapshot of all processes in the system
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create process snapshot. Error code: " << GetLastError() << std::endl;
        return 0;
    }

    // Iterate through the processes to find the one with the given name
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe32))
    {
        CloseHandle(hSnapshot);
        std::cerr << "Failed to get first process. Error code: " << GetLastError() << std::endl;
        return 0;
    }

    do {
        // Compare process names in a case-insensitive manner
        std::wstring currentProcessName = pe32.szExeFile;
        std::transform(currentProcessName.begin(), currentProcessName.end(), currentProcessName.begin(), ::tolower);
        std::wstring searchNameLower = processName;
        std::transform(searchNameLower.begin(), searchNameLower.end(), searchNameLower.begin(), ::tolower);

        if (currentProcessName == searchNameLower) {
            CloseHandle(hSnapshot);
            return pe32.th32ProcessID; // Return the process ID if found
        }
    } while (Process32Next(hSnapshot, &pe32));

    // If the process is not found, return 0
    CloseHandle(hSnapshot);
    return 0;
}

// Function to perform signature scanning
uintptr_t FindPattern(uintptr_t startAddress, size_t size, const char* pattern) {
    size_t patternLength = strlen(pattern);
    const char* patternEnd = pattern + patternLength;

    for (uintptr_t i = 0; i < size - patternLength; ++i) {
        bool found = true;
        for (size_t j = 0; j < patternLength; ++j) {
            if (pattern[j] != '?' && pattern[j] != *reinterpret_cast<const char*>(startAddress + i + j)) {
                found = false;
                break;
            }
        }

        if (found) {
            return startAddress + i;
        }
    }

    return 0;
}

int findpatt(const char* pattern)
{
    // Example: searching for a pattern in the current process's memory
    MODULEINFO moduleInfo;
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &moduleInfo, sizeof(moduleInfo));

    // Define the pattern to search for
    //const char* pattern = "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x18\x56";

    // Perform the signature scan
    uintptr_t address = FindPattern(reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll), moduleInfo.SizeOfImage, pattern);
    if (address != 0) {
        std::cout << "Pattern found at address: 0x" << std::hex << address << std::endl;
    }
    else {
        std::cout << "Pattern not found." << std::endl;
    }

    return 0;
}

int getProc()
{
    HMODULE hModule = LoadLibrary(L"Example.dll");
    if (hModule == NULL) {
        std::cerr << "Failed to load DLL." << std::endl;
        return 1;
    }

    // Get the address of the function
    FARPROC functionAddress = GetProcAddress(hModule, "FunctionName");
    if (functionAddress == NULL) {
        std::cerr << "Failed to get the address of the function." << std::endl;
        FreeLibrary(hModule); // Free the DLL
        return 1;
    }

    // Define a function pointer with the correct signature
    typedef void (*FunctionType)();
    FunctionType myFunction = reinterpret_cast<FunctionType>(functionAddress);

    // Call the function
    myFunction();

    // Free the DLL
    FreeLibrary(hModule);

    return 0;
}

// Helper function to enumerate exported functions in a module
void EnumerateExportedFunctions(HMODULE hModule)
{
    // Get the module name
    TCHAR moduleName[MAX_PATH];
    if (GetModuleFileName(hModule, moduleName, MAX_PATH) == 0)
    {
        std::cerr << "Failed to get module name." << std::endl;
        return;
    }

    std::cout << "Exported functions in module "  << (char)moduleName << ":" << std::endl;

    // Enumerate exported functions
    DWORD dwSize;
    PIMAGE_EXPORT_DIRECTORY pExportDirectory = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(ImageDirectoryEntryToData(hModule, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &dwSize));

    if (pExportDirectory == nullptr)
    {
        std::cerr << "Failed to get export directory." << std::endl;
        return;
    }

    DWORD* pFunctions = reinterpret_cast<DWORD*>(reinterpret_cast<BYTE*>(hModule) + pExportDirectory->AddressOfFunctions);
    for (DWORD i = 0; i < pExportDirectory->NumberOfFunctions; ++i)
    {
        // Get the function name
        char functionName[256];
        if (IMAGE_ORDINAL_FLAG32 & pFunctions[i])
        {
            // The function is exported by ordinal
            sprintf_s(functionName, "#%lu", pFunctions[i] - pExportDirectory->Base);
        }
        else
        {
            // The function is exported by name
            PSTR pszFunctionName = reinterpret_cast<PSTR>(reinterpret_cast<BYTE*>(hModule) +
                reinterpret_cast<DWORD*>(reinterpret_cast<BYTE*>(hModule) + pExportDirectory->AddressOfNames)[i]);
            strcpy_s(functionName, pszFunctionName);
        }
        std::cout << functionName << std::endl;
    }
}

// Helper function to enumerate all modules (including DLLs) loaded in a process
void EnumerateModules(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess == NULL)
    {
        std::cerr << "Failed to open process." << std::endl;
        return;
    }

    HMODULE hModules[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded))
    {
        int moduleCount = cbNeeded / sizeof(HMODULE);
        for (int i = 0; i < moduleCount; ++i)
        {
            EnumerateExportedFunctions(hModules[i]);
        }
    }
    else
    {
        std::cerr << "Failed to enumerate process modules." << std::endl;
    }

    CloseHandle(hProcess);
}

bool InjectDLL(DWORD processID, const char* dllPath)
{
    // Open target process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processID);
    if (hProcess == NULL)
    {
        std::cout << "Failed to open process. Error code: " << GetLastError() << std::endl;
        return false;
    }

    // Allocate memory for the DLL path in the target process
    LPVOID dllPathAddr = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (dllPathAddr == NULL)
    {
        std::cout << "Failed to allocate memory in the target process. Error code: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    // Write the DLL path to the allocated memory
    if (!WriteProcessMemory(hProcess, dllPathAddr, dllPath, strlen(dllPath) + 1, NULL)) {
        std::cout << "Failed to write DLL path to target process memory. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, dllPathAddr, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Get the address of the LoadLibraryA function in the target process
    LPVOID loadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
    if (loadLibraryAddr == NULL) {
        std::cout << "Failed to get address of LoadLibraryA function. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, dllPathAddr, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Create a remote thread in the target process to load the DLL
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, dllPathAddr, 0, NULL);
    if (hThread == NULL) {
        std::cout << "Failed to create remote thread in target process. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, dllPathAddr, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Wait for the remote thread to finish
    WaitForSingleObject(hThread, INFINITE);

    // Clean up
    VirtualFreeEx(hProcess, dllPathAddr, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    std::cout << "DLL injected successfully." << std::endl;
    return true;
}

void getfile_in_Folder(std::string folderpath)
{

    try
    {
        for (const auto& entry : fs::directory_iterator(folderpath))
        {
            if (fs::is_regular_file(entry))
            {
                std::cout << entry.path().filename() << '\n';
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }

}

int CustomFunction(int arg)
{
    std::cout << "CustomFunction called with arg: " << arg << std::endl;
    // Call the original function
    mainfunType pfnOriginalFunction = (mainfunType)mainfun;
    return pfnOriginalFunction(arg);
}

void HookFunction(void* pFunction, void* pDetour)
{
    DWORD oldProtect;
    VirtualProtect(pFunction, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

    // Create a relative jump to the detour function
    DWORD relativeAddress = ((DWORD)pDetour - (DWORD)pFunction - 5);
    *(BYTE*)pFunction = 0x00; // JMP opcode
    memcpy((BYTE*)pFunction + 1, &relativeAddress, sizeof(DWORD));

    VirtualProtect(pFunction, 5, oldProtect, &oldProtect);
}
