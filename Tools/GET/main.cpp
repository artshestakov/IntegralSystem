#include <iostream>
#include <string>
#include <cstdio>
//-----------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#else
#include <time.h>
#endif
//-----------------------------------------------------------------------------
void GetDate();
void GetTime();
//-----------------------------------------------------------------------------
int main(int argc, char ** argv)
{
    bool Result = argc > 1;
    if (Result)
    {
        std::string Argument = argv[1];
        if (Argument == "--getdate")
        {
            GetDate();
        }
        else if (Argument == "--gettime")
        {
            GetTime();
        }
        else
        {
            std::cout << "Error: invalid argument\n" << std::endl;
            Result = false;
        }
    }
    else
    {
        std::cout << "Error: not specified arguments." << std::endl;
        std::cout << "Using: GET" << std::endl;
#ifdef WIN32
        std::cout << ".exe" << std::endl;
#endif
        std::cout << " [ARGUMENT]" << std::endl;
        std::cout << "Arguments:" << std::endl;
        std::cout << "--getdate - get current date" << std::endl;
        std::cout << "--gettime - get current time" << std::endl;
    }
    return Result ? EXIT_SUCCESS : EXIT_FAILURE;
}
//-----------------------------------------------------------------------------
void GetDate()
{
#ifdef WIN32
    SYSTEMTIME ST;
    GetLocalTime(&ST);
    printf("%02d.%02d.%02d", ST.wDay, ST.wMonth, ST.wYear);
#else
    time_t Time = time(NULL);
    struct tm TM = *localtime(&Time);
    printf("%02d.%02d.%02d", TM.tm_mday, TM.tm_mon + 1, TM.tm_year + 1900);
#endif
}
//-----------------------------------------------------------------------------
void GetTime()
{
#ifdef WIN32
    SYSTEMTIME ST;
    GetLocalTime(&ST);
    printf("%02d:%02d", ST.wHour, ST.wMinute);
#else
    time_t Time = time(NULL);
    struct tm TM = *localtime(&Time);
    printf("%02d:%02d", TM.tm_hour, TM.tm_min);
#endif
}
//-----------------------------------------------------------------------------
