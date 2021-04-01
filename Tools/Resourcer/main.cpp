#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#ifdef WIN32
#include <shlwapi.h>
#endif
//-----------------------------------------------------------------------------
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
    size_t SeparatorIndex = DirPath.size();

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
        std::cout << "Not specified dir path" << std::endl;
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
        std::cout << "Path \"" << DirPath << "\" not exist" << std::endl;
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
        std::cout << "No open path (" << DirPathTemp << "): " << GetErrorString() << std::endl;
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
    FILE *FileOut = nullptr;
    errno_t Error = fopen_s(&FileOut, "G:\\out", "wb");
    if (Error != 0)
    {
        std::cout << "Error open out file: " << GetErrorString() << std::endl;
        return false;
    }

    //Засекаем время
    auto TimeStart = std::chrono::steady_clock::now();

    for (size_t i = 0, c = VectorFiles.size(); i < c; ++i)
    {
        std::string FilePath = VectorFiles[i];
        printf("Reading file (%zd of %zd): %s\n", i + 1, c, FilePath.c_str());
        if (!ReadFile(FilePath, SeparatorIndex, FileOut))
        {
            return false;
        }
    }
    fclose(FileOut);
    printf("Complete with %llu msec\n", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - TimeStart).count());
    return true;
}
//-----------------------------------------------------------------------------
bool ReadFile(const std::string &FilePath, size_t &SeparatorIndex, FILE *FileOut)
{
    FILE *File = nullptr;
    errno_t Error = fopen_s(&File, FilePath.c_str(), "rb");
    if (Error != 0) //Не удалось открыть файл
    {
        std::cout << "Error open file: " + GetErrorString() << std::endl;
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
                long Readed = (long)fread_s(Data, FileSize, sizeof(unsigned char), FileSize, File);
                Result = Readed == FileSize;
                if (Result) //Файл прочитан успешно
                {
                    //Копируем путь к файлу и отрезаем лишнее
                    std::string Temp = FilePath;
                    Temp.erase(0, SeparatorIndex);

                    //Записываем заголовок файла
                    fprintf_s(FileOut, "FileName=%s Size=%ld\n", Temp.c_str(), FileSize);

                    //Записываем содержимое текущего файла в выходной файл
                    Result = (long)fwrite(Data, sizeof(unsigned char), FileSize, FileOut) == FileSize &&
                        (long)fwrite("\n", sizeof(unsigned char), 1, FileOut) == 1;
                    if (Result) //Ошибка записи
                    {
                        if (fflush(FileOut) != 0)
                        {
                            std::cout << "No flushing data to out file: " << GetErrorString() << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Error write: " << GetErrorString() << std::endl;
                    }
                }
                else //Ошибка чтения файла
                {
                    std::cout << "Error read file: " << GetErrorString() << std::endl;
                }
            }
            else //Не удалось выделить память
            {
                std::cout << "Malloc error" << std::endl;
            }
        }
        else //Не удалось получить размер файла
        {
            std::cout << "Not getting file size: " << GetErrorString() << std::endl;
        }
    }
    else //Не удалось переместиться в конец файла
    {
        std::cout << "Not seek to end file: " << GetErrorString() << std::endl;
    }

    //Закрываем файл и выходим
    fclose(File);
    return Result;
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
    char Buffer[BUFFER_ERROR_SIZE];
    (void)strerror_s(Buffer, BUFFER_ERROR_SIZE, errno);
    ErrorString = Buffer;
#endif
    return ErrorString;
}
//-----------------------------------------------------------------------------
