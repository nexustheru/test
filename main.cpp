
#include "worker.h"
#include "render.h"

int main()
{
  //render();

    std::wstring processName = L"notepad++.exe"; // Replace this with the process name you want to find
   DWORD processId = GetProcessIdByName(processName);
    if (processId) 
    {
        std::wcout << L"Process found! Process ID: " << processId << std::endl;
        HMODULE hModule = GetModuleHandle(L"notepad++.exe");
        uintptr_t baseAddress = reinterpret_cast<uintptr_t>(hModule);
        std::cout << "Base Address: 0x" << std::hex << baseAddress << std::endl;
    }
    else
    {
        std::wcerr << L"Process not found." << std::endl;
    }
   
   
  system("PAUSE");
  return 0;
}