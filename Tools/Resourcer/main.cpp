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
#define LOG_I(MESSAGE) std::cout << "INFO: " << MESSAGE << std::endl;
#define LOG_W(MESSAGE) std::cout << "WARNING: " << MESSAGE << std::endl;
#ifdef WIN32
const char PATH_SEPARATOR = '\\';
#else
const char PATH_SEPARATOR = '/';
#endif
const size_t BUFFER_ERROR_SIZE = 1024;
//-----------------------------------------------------------------------------
bool CheckArgument(int argc, char** argv, std::string &DirPath); //Проверка аргумента
void PreparePath(std::string &DirPath); //Подготовка пути
bool RecursiveSearch(const std::string &DirPath, std::vector<std::string> &VectorFiles); //Чтение содержимого папки
bool ReadFiles(std::vector<std::string> &VectorFiles, size_t &SeparatorIndex); //Чтение файлов
bool ReadFile(const std::string &FilePath, size_t &SeparatorIndex, FILE *FileOut); //Чтение файла
std::string GetErrorString(); //Получить текст последней ошибки
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
    size_t SeparatorIndex = DirPath.size() - 1;

    //Читаем директорию
    std::vector<std::string> VectorFiles;
    if (!RecursiveSearch(DirPath.c_str(), VectorFiles))
    {
        return EXIT_FAILURE;
    }

    if (!ReadFiles(VectorFiles, SeparatorIndex))
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
bool ReadFiles(std::vector<std::string> &VectorFiles, size_t &SeparatorIndex)
{
    remove("G:\\out");

    FILE *FileOut = nullptr;
    errno_t Error = fopen_s(&FileOut, "G:\\out", "wb");
    if (Error != 0)
    {
        LOG_E("Error open file: " + GetErrorString());
        return false;
    }

    for (const std::string &FilePath : VectorFiles)
    {
        LOG_I("Reading file " + FilePath);
        if (!ReadFile(FilePath, SeparatorIndex, FileOut))
        {
            return false;
        }
    }
    fclose(FileOut);
    return true;
}
//-----------------------------------------------------------------------------
bool ReadFile(const std::string &FilePath, size_t &SeparatorIndex, FILE *FileOut)
{
    FILE *File = nullptr;
    errno_t Error = fopen_s(&File, FilePath.c_str(), "rb");
    if (Error != 0) //Не удалось открыть файл
    {
        LOG_E("Not open file: " + GetErrorString());
        return false;
    }

    //Перемещаемся в конец файла
    int FSeek = fseek(File, 0L, SEEK_END);
    bool Result = FSeek == 0;
    if (Result) //Перемещение прошло успешно
    {
        long FileSize = ftell(File);
        Result = FileSize != 1L;
        if (Result) //Размер файла получен
        {
            rewind(File); //Возвращаемся в начало файла

            //Выделяем память под содержмое
            unsigned char *Data = (unsigned char *)malloc(FileSize + 1);
            Result = Data ? true : false;
            if (Result) //Память успешно выделена
            {
                long Readed = fread_s(Data, FileSize, sizeof(unsigned char), FileSize, File);
                Result = Readed == FileSize;
                if (Result) //Файл прочитан успешно
                {
                    //Копируем путь к файлу и отрезаем лишнее
                    std::string Temp = FilePath;
                    Temp.erase(0, SeparatorIndex);

                    fprintf_s(FileOut, "FileName=%s\n", Temp.c_str());
                    //Записываем содержимое текущего файла в выходной файл
                    Result = (long)fwrite(Data, sizeof(unsigned char), FileSize, FileOut) == FileSize &&
                        (long)fwrite("\n", sizeof(unsigned char), 1, FileOut) == 1;
                    if (Result) //Ошибка записи
                    {
                        if (fflush(FileOut) != 0)
                        {
                            LOG_W("No flushing data");
                        }
                    }
                    else
                    {
                        LOG_E("Error write");
                    }
                }
                else //Ошибка чтения файла
                {
                    LOG_E("Error read file: " + GetErrorString());
                }
            }
            else //Не удалось выделить память
            {
                LOG_E("Malloc error: " + GetErrorString());
            }
        }
        else //Не удалось получить размер файла
        {
            LOG_E("Not getting file size: " + GetErrorString());
        }
    }
    else //Не удалось переместиться в конец файла
    {
        LOG_E("Not seek to end file: " + GetErrorString());
    }

    //Закрываем файл и выходим
    fclose(File);
    return Result;
}
//-----------------------------------------------------------------------------
std::string GetErrorString()
{
    char Buffer[BUFFER_ERROR_SIZE];
    (void)strerror_s(Buffer, BUFFER_ERROR_SIZE, errno);
    return Buffer;
}
//-----------------------------------------------------------------------------
