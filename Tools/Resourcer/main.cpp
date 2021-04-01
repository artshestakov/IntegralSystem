#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <vector>
#ifdef WIN32
#include <shlwapi.h>
#endif
//-----------------------------------------------------------------------------
#define LOG_E(MESSAGE) std::cout << "ERROR: " << MESSAGE << std::endl;
#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif
//-----------------------------------------------------------------------------
bool CheckArgument(int argc, char** argv, std::string &DirPath); //Проверка аргумента
void PreparePath(std::string &DirPath); //Подготовка пути
bool RecursiveSearch(const std::string &DirPath, std::vector<std::string> &VectorFiles); //Чтение содержимого папки
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    //Проверим аргумент
    std::string DirPath;
    if (!CheckArgument(argc, argv, DirPath))
    {
        return EXIT_FAILURE;
    }
    PreparePath(DirPath);

    //Читаем директорию
    std::vector<std::string> VectorFiles;
    if (!RecursiveSearch(DirPath.c_str(), VectorFiles))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
bool CheckArgument(int argc, char** argv, std::string &DirPath)
{
    //Проверим, есть ли вообще аргумент
    if (argc < 2) //Аргумента нет - выводим ошибку, помощь и выходим
    {
        LOG_E("Not specified dir path");
#ifdef WIN32
        std::cout << "Example: Resourcer.exe C:\\path\\to\\dir";
#else
        std::cout << "Example: ./Resourcer /path/to/dir";
#endif
        std::cout << std::endl;
        return false;
    }
    DirPath = argv[1];

    //Проверим существование такого пути
#ifdef WIN32
    bool PathExist = PathFileExists(DirPath.c_str()) == TRUE;
#else
    bool PathExist = access(DirPath.c_str(), F_OK) == 0;
#endif
    if (!PathExist) //Нет такого пути - ошибка
    {
        LOG_E("Path \"" + DirPath + "\" not exist");
        return false;
    }

#ifdef WIN32
    DWORD Attributes = GetFileAttributes(DirPath.c_str());
    bool PathIsDir = (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
#endif
    if (!PathIsDir)
    {
        std::cout << "Path \"" + DirPath + "\" is not a dir" << std::endl;
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
void PreparePath(std::string &DirPath)
{
    if (DirPath.back() != PATH_SEPARATOR)
    {
        DirPath.push_back(PATH_SEPARATOR);
    }
}
//-----------------------------------------------------------------------------
bool RecursiveSearch(const std::string &DirPath, std::vector<std::string> &VectorFiles)
{
    WIN32_FIND_DATA FindData = { 0 };
    std::string DirPathTemp = DirPath + '*';
    HANDLE Handle = FindFirstFile(DirPathTemp.c_str(), &FindData);
    if (Handle == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    do
    {
        //Пропускаем указатели на текущий и родитеский каталоги
        if ((strcmp(FindData.cFileName, ".") == 0) || (strcmp(FindData.cFileName, "..") == 0))
        {
            continue;
        }
        
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //Директория
        {
            if (!RecursiveSearch(DirPath + std::string(FindData.cFileName) + PATH_SEPARATOR, VectorFiles))
            {
                return false;
            }
        }
        else //Файл
        {
            VectorFiles.emplace_back(DirPath + std::string(FindData.cFileName));
        }
    } while (FindNextFile(Handle, &FindData));
    FindClose(Handle);
    return true;
}
//-----------------------------------------------------------------------------
