#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>


using namespace std;


class Loggin
{
public:
    Loggin()
    {
       file= ofstream("Loggin.txt");
    }
    void logtofile(string txt)
    {

    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
      if (file.is_open())
      {
        //file << (char)now_time_t << " " << txt << std::endl;
        file << txt << std::endl;
        file.close();
      }
      else cout << "Unable to open file";
    }
private:
    ofstream file;
};


