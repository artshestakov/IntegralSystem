#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include "ISArguments.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#include <Shlwapi.h>
#pragma warning(disable: 4702)
#else
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
//-----------------------------------------------------------------------------
#include "System.h"
//-----------------------------------------------------------------------------
bool IsVerbose = false;
std::string PathDir;
std::string PathFileOut;
//-----------------------------------------------------------------------------
bool CheckDirPath(); //Проверка аргументов
bool GetFilesPath(const std::string &DirPath, std::vector<std::string> &VectorFiles); //Чтение содержимого папки
bool ReadFiles(const std::vector<std::string> &VectorFiles, size_t PathDirSize);
bool ReadFile(const std::string &PathFile, FILE *FileOut, size_t PathDirSize);
std::string GetErrorString(); //Получить текст последней ошибки
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    //Установим кодироку на всякий случай
    setlocale(LC_ALL, "Russian");

    ISArguments CMD;
    CMD.AddFlag("-h", "--help", "Show this message");
    CMD.AddFlag("-v", "--verbose", "Verbose message mode");
    CMD.AddParameter("-d", "--dir", "Resource directory");
    CMD.AddParameter("-o", "--output", "Output file path");
    if (!CMD.Parse(argc, argv))
    {
        printf("%s\n", CMD.GetErrorString().c_str());
        return EXIT_FAILURE;
    }

    if (CMD.IsExist("-h"))
    {
        printf("RCC - resource generation utility.\n");
        printf("%s\n", CMD.GetHelp().c_str());
        return EXIT_SUCCESS;
    }

    //Получим значения аргументов
    IsVerbose = CMD.IsExist("-v");
    PathDir = CMD.GetValue("-d");
    PathFileOut = CMD.GetValue("-o");

    //Проверим папку с ресурсами
    if (!CheckDirPath())
    {
        return EXIT_FAILURE;
    }

    //Читаем директорию
    std::vector<std::string> VectorFiles;
    if (!GetFilesPath(PathDir.c_str(), VectorFiles))
    {
        return EXIT_FAILURE;
    }

    //Читаем файлы
    if (!ReadFiles(VectorFiles, PathDir.size()))
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
bool CheckDirPath()
{
    //Проверим существование такого пути
#ifdef WIN32
    bool PathExist = PathFileExists(PathDir.c_str()) == TRUE;
#else
    bool PathExist = access(PathDir.c_str(), F_OK) == 0;
#endif
    if (!PathExist) //Нет такого пути - ошибка
    {
        printf("Path \"%s\" not exist\n", PathDir.c_str());
        return false;
    }

    //Проверим, что путь ведёт к папке
#ifdef WIN32
    DWORD Attributes = GetFileAttributes(PathDir.c_str());
    bool PathIsDir = (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat StatBUF;
    if (stat(PathDir.c_str(), &StatBUF) != 0)
    {
        printf("Error checking path: %s\n", GetErrorString().c_str());
        return false;
    }
    bool PathIsDir = S_ISDIR(StatBUF.st_mode);
#endif
    if (!PathIsDir)
    {
        printf("Path \"%s\" is not a dir\n", PathDir.c_str());
        return false;
    }

    //Добавим слеш в конец, если его нет
    if (PathDir.back() != PATH_SEPARATOR)
    {
        PathDir.push_back(PATH_SEPARATOR);
    }
    return true;
}
//-----------------------------------------------------------------------------
bool GetFilesPath(const std::string &DirPath, std::vector<std::string> &VectorFiles)
{
    //Вытаскиваем 

#ifdef WIN32
    WIN32_FIND_DATA FindData = { 0 };
    std::string DirPathTemp = DirPath + '*';
    HANDLE Handle = FindFirstFile(DirPathTemp.c_str(), &FindData);
    if (Handle == INVALID_HANDLE_VALUE)
    {
        printf("Error open path (%s): %s\n", DirPathTemp.c_str(), GetErrorString().c_str());
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
            if (!GetFilesPath(DirPath + std::string(FindData.cFileName) + PATH_SEPARATOR, VectorFiles))
            {
                return false;
            }
        }
        else //Файл
        {
            VectorFiles.emplace_back(DirPath + FindData.cFileName);
        }
    } while (FindNextFile(Handle, &FindData));
    FindClose(Handle);
#else
    DIR *Dir = { 0 };
    struct dirent *Entry = { 0 };

    if (!(Dir = opendir(DirPath.c_str())))
    {
        return false;
    }

    while ((Entry = readdir(Dir)) != NULL)
    {
        if (Entry->d_type == DT_DIR) //Директория
        {
            //Пропускаем указатели на текущий и родитеский каталоги
            if (strcmp(Entry->d_name, ".") == 0 || strcmp(Entry->d_name, "..") == 0)
            {
                continue;
            }

            if (!GetFilesPath(DirPath + Entry->d_name + '/', VectorFiles))
            {
                return false;
            }
        }
        else //Файл
        {
            VectorFiles.emplace_back(DirPath + Entry->d_name);
        }
    }
    closedir(Dir);
#endif
    return true;
}
//-----------------------------------------------------------------------------
bool ReadFiles(const std::vector<std::string> &VectorFiles, size_t PathDirSize)
{
    if (IsVerbose)
    {
        printf("Reading files...\n");
    }

    //Откроем выходной файл
    FILE *FileOut = fopen(PathFileOut.c_str(), "w");
    if (!FileOut)
    {
        printf("Error create|open out file: %s\n", PathFileOut.c_str());
        return false;
    }

    //Добавим шапку файла
    fprintf(FileOut, "/****************************************************************************\n");
    fprintf(FileOut, "** Resource object code\n");
    fprintf(FileOut, "**\n");
    fprintf(FileOut, "** Created by: The Resource Compiler (RCC)\n");
    fprintf(FileOut, "**\n");
    fprintf(FileOut, "*****************************************************************************/\n");
    fprintf(FileOut, "\n");
    fprintf(FileOut, "#pragma once\n");
    fprintf(FileOut, "#ifndef _RCC_H_INCLUDED\n");
    fprintf(FileOut, "#define _RCC_H_INCLUDED\n");
    fprintf(FileOut, "\n");
    fprintf(FileOut, "namespace RCC\n");
    fprintf(FileOut, "{");

    auto TimeStart = std::chrono::steady_clock::now();
    for (const std::string &PathFile : VectorFiles)
    {
        if (!ReadFile(PathFile, FileOut, PathDirSize))
        {
            return false;
        }
    }

    //Добавим закрывающую фигурную скобку, получим размер файла и закроем его
    fprintf(FileOut, "}\n");
    fprintf(FileOut, "#endif\n");
    long FileOutSize = ftell(FileOut);
    fflush(FileOut);
    fclose(FileOut);

    printf("Complete with " CHRONO_FORMAT " msec. Files: %zu. Size: %ld (%.3f MB).\n"
        "Output file: %s\n",
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - TimeStart).count(),
        VectorFiles.size(), FileOutSize, ((double)FileOutSize / (double)1000) / (double)1024, PathFileOut.c_str());
    return true;
}
//-----------------------------------------------------------------------------
bool ReadFile(const std::string &PathFile, FILE *FileOut, size_t PathDirSize)
{
    if (IsVerbose)
    {
        printf("Reading file \"%s\"...\n", PathFile.c_str());
    }

    //Пытаемся открыть файл
    FILE *File = fopen(PathFile.c_str(), "rb");
    if (!File)
    {
        printf("Error open file \"%s\": %s\n", PathFile.c_str(), GetErrorString().c_str());
        fclose(File);
        return false;
    }

    //Смещаемся в конец
    if (fseek(File, 0, SEEK_END) != 0)
    {
        printf("Error fseek(): %s\n", GetErrorString().c_str());
        fclose(File);
        return false;
    }

    //Получаем размер файла
    long FileSize = (size_t)ftell(File);
    if (FileSize == -1)
    {
        printf("Error ftell(): %s\n", GetErrorString().c_str());
        fclose(File);
        return false;
    }

    //Возвращаемся в начало файла
    rewind(File);

    //Выделяем память
    unsigned char *Data = (unsigned char *)malloc((size_t)FileSize + 1);
    if (!Data)
    {
        printf("Error malloc(): %s\n", GetErrorString().c_str());
        fclose(File);
        return false;
    }

    //Читаем файл в память
    size_t Readed = fread(Data, sizeof(unsigned char), (size_t)FileSize, File);
    if (Readed != (size_t)FileSize)
    {
        printf("Error read. Readed: %zu, File size: %ld\n", Readed, FileSize);
        free(Data);
        fclose(File);
        return false;
    }

    //Закрываем за собой файл
    fclose(File);

    //Формируем имя переменной
    std::string Temp = PathFile.substr(PathDirSize, PathFile.size() - PathDirSize);
    std::replace(Temp.begin(), Temp.end(), PATH_SEPARATOR, '_');
    std::replace(Temp.begin(), Temp.end(), '.', '_');
    std::transform(Temp.begin(), Temp.end(), Temp.begin(), toupper);
    const char *VariableName = Temp.c_str();

    //Добавим объявление переменной
    fprintf(FileOut, "\n\tstatic const unsigned char %s[] =\n\t{\n\t\t", VariableName);

    for (long i = 0, FiendIndex = 1; i < FileSize; ++i, ++FiendIndex)
    {
        fprintf(FileOut, "0x%02X,", Data[i]);
        if (FiendIndex == 16)
        {
            fprintf(FileOut, "\n\t\t");
            FiendIndex = 0;
        }
    }

    //Закроем фигурную скобку
    fprintf(FileOut, "\n\t};\n");

    //Добавим размер переменной и имя
    fprintf(FileOut, "\tstatic const int %s_SIZE = %ld;\n", VariableName, FileSize);
    fprintf(FileOut, "\tstatic const char %s_NAME[] = \"%s\";\n", VariableName, VariableName);
    fflush(FileOut);
    return true;
}
//-----------------------------------------------------------------------------
std::string GetErrorString()
{
    std::string ErrorString = "Unknown error";
#ifdef WIN32
    DWORD ErrorID = GetLastError();
    if (ErrorID != 0) //Код ошибки валиден
    {
        LPSTR Buffer = nullptr;
        size_t MessageSize = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ErrorID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&Buffer, 0, NULL);
        if (MessageSize > 0 && Buffer)
        {
            ErrorString = std::string(Buffer, MessageSize - 2);
            LocalFree(Buffer);
        }
    }
#else
    ErrorString = strerror(errno);
#endif
    return ErrorString;
}
//-----------------------------------------------------------------------------
