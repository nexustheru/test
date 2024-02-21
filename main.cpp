
#include "worker.h"
#include "render.h"


#include <windows.h>
#include <iostream>
#include <print>
#include <thread>
using namespace std;


int main()
{


    //DWORD processId = GetProcessIdByName(L"programtoinject.exe");
    //const char* dllPath = "testdll.dll";
    //if (processId) 
    //{
   // compileruntime();
   // monoInit();
   // monoclean();
    /*auto mypath = "E:/vs projects/test/x64/Release";
    getfile_in_Folder(mypath);*/
    //}
    //else
    //{
        //std::cout << "Process not found." << std::endl;
    //}
   
    std::thread t(render);
    t.join();
  
  system("PAUSE");
  return 0;
}