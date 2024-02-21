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
#include <mono/metadata/debug-helpers.h>
#include <filesystem>
#include "log.h"

namespace fs = std::filesystem;
static int malloc_count = 0;

class Monoi
{
public:

   Monoi();
   void Update();
   void Get_Scripts();
   void Get_Dlls();

   void Init();
   bool compile();
   void sorting();
   void listfunc();
   void createFunction(MonoImage* image,std::string classname, std::string classnameandMethod, void* args[]);

   void GetAssemblyClassList(MonoImage* image);
   void output_methods(MonoClass* klass);
   void* custom_malloc(size_t bytes);
   void testiter1(MonoClass* klass);
   bool isRunning = false;
  //plugin methods
   void PluginOnCreate(void* args[]);
   void PluginOnUpdate(void* args[]);
   void PluginOnLoad(void* args[]);
   void PluginOnExit(void* args[]);
   void PluginReturnName(void* args[]);
   void PluginsetPlugiName(void* args[]);
   
   ~Monoi();

private:

  MonoDomain* domain;
  MonoAssembly* assembly;
  MonoImage* image;
  Loggin* log;




  std::vector<std::string>* dlls;
  std::vector<std::string>* scripts;
  std::vector<MonoClass*>* classes;
  std::vector<MonoAssembly*>* assemblys;
  std::vector<MonoImage*>* images;
};


