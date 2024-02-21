#pragma once

#include <iostream>
#include <lua.hpp>
#include <filesystem>

namespace fs = std::filesystem;

lua_State* L;

bool loadscripts()
{
        std::string folderpath = "Scripts/";
        try
        {
            for (const auto& entry : fs::directory_iterator(folderpath))
            {
                if (fs::is_regular_file(entry))
                {
                    if (entry.path().filename().extension().string() == ".lua")
                    {                  
                        auto temp = folderpath + entry.path().filename().string();
                        
                        if (luaL_loadfile(L, temp.c_str()) || lua_pcall(L, 0, 0, 0))
                        {
                            std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
                            lua_close(L);
                            return false;
                        }

                    }
                   
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() <<  " Lua:" << lua_tostring(L, -1) << std::endl;
            return false;
        }

  
    return true;
}
bool loadfunction(const char* functioname)
{
    lua_getglobal(L, functioname);
    lua_pushnumber(L, 5);
    lua_pushnumber(L, 3);
    if (lua_pcall(L, 2, 1, 0) != LUA_OK)
    {
        std::cerr << "Error calling add: " << lua_tostring(L, -1) << std::endl;
        lua_close(L);
        return false;
    }
    int result = static_cast<int>(lua_tonumber(L, -1));
    std::cout << "Result of add function: " << result << std::endl;
    return true;
}
int initlua() 
{
    // Create a Lua state
    L = luaL_newstate();
    luaL_openlibs(L); // Load Lua libraries

    // Execute a Lua script
    bool res = loadscripts();
    // Call a Lua function from C++
    bool res1 = loadfunction("add");

    // Close Lua state
    lua_close(L);

    return 0;
}
