#include <cstdlib>
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <chrono>
#ifdef WIN32
#include <shlwapi.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
//-----------------------------------------------------------------------------
#ifdef WIN32
const char PATH_SEPARATOR = '\\';
#define CHRONO_FORMAT "%lld"
#else
const char PATH_SEPARATOR = '/';
#define CHRONO_FORMAT "%ld"
#endif
const char OUTPUT_FILE_NAME[] = "Resources.bin";
//-----------------------------------------------------------------------------
bool CheckArgument(int argc, char** argv, std::string &DirPath, std::string &ApplicationDir); //Проверка аргумента
void PreparePath(std::string &DirPath); //Подготовка пути
bool GetFilesPath(const std::string &DirPath, std::vector<std::string> &VectorFiles); //Чтение содержимого папки
bool ReadFiles(std::vector<std::string> &VectorFiles, size_t &SeparatorIndex, std::string &ApplicationDir); //Чтение файлов
bool ReadFile(const std::string &FilePath, size_t &SeparatorIndex, unsigned long long &FileOutSize, FILE *FileOut); //Чтение файла
std::string GetErrorString(); //Получить текст последней ошибки
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    //Установим кодироку на всякий случай
    setlocale(LC_ALL, "Russian");

    std::string DirPath, ApplicationDir;

    //Проверим аргумент
    if (!CheckArgument(argc, argv, DirPath, ApplicationDir))
    {
        return EXIT_FAILURE;
    }

    PreparePath(DirPath);
    size_t SeparatorIndex = DirPath.size();

    //Читаем директорию
    printf("Getting files list...\n");
    std::vector<std::string> VectorFiles;
    if (!GetFilesPath(DirPath.c_str(), VectorFiles))
    {
        return EXIT_FAILURE;
    }

    if (!ReadFiles(VectorFiles, SeparatorIndex, ApplicationDir))
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
bool CheckArgument(int argc, char** argv, std::string &DirPath, std::string &ApplicationDir)
{
    //Проверим, есть ли вообще аргумент
    if (argc < 2) //Аргумента нет - выводим ошибку, помощь и выходим
    {
        printf("Not specified dir path\n");
#ifdef WIN32
        printf("Example: Resourcer.exe C:\\path\\to\\dir\n");
#else
        printf("Example: ./Resourcer /path/to/dir\n");
#endif
        return false;
    }
    DirPath = argv[1];

    ApplicationDir = argv[0];
    size_t Pos = ApplicationDir.rfind(PATH_SEPARATOR);
    if (Pos != std::string::npos)
    {
        ApplicationDir.erase(Pos + 1, ApplicationDir.size() - Pos);
    }

    //Проверим существование такого пути
#ifdef WIN32
    bool PathExist = PathFileExists(DirPath.c_str()) == TRUE;
#else
    bool PathExist = access(DirPath.c_str(), F_OK) == 0;
#endif
    if (!PathExist) //Нет такого пути - ошибка
    {
        printf("Path \"%s\" not exist\n", DirPath.c_str());
        return false;
    }

#ifdef WIN32
    DWORD Attributes = GetFileAttributes(DirPath.c_str());
    bool PathIsDir = (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat StatBUF;
    if (stat(DirPath.c_str(), &StatBUF) != 0)
    {
        printf("Error checking path: %s\n", GetErrorString().c_str());
        return false;
    }
    bool PathIsDir = S_ISDIR(StatBUF.st_mode);
#endif
    if (!PathIsDir)
    {
        printf("Path \"%s\" is not a dir\n", DirPath.c_str());
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
bool GetFilesPath(const std::string &DirPath, std::vector<std::string> &VectorFiles)
{
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
            VectorFiles.emplace_back(DirPath + std::string(FindData.cFileName));
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
bool ReadFiles(std::vector<std::string> &VectorFiles, size_t &SeparatorIndex, std::string &ApplicationDir)
{
    //Проверим наличие выходного файла
    std::string PathOutputFile = ApplicationDir + OUTPUT_FILE_NAME;
#ifdef WIN32
    bool IsExist = PathFileExists(PathOutputFile.c_str()) == TRUE;
#else
    bool IsExist = access(PathOutputFile.c_str(), F_OK) == 0;
#endif
    if (IsExist) //Если файл существует - пробуем удалить его
    {
#ifdef WIN32
        bool Deleted = DeleteFile(PathOutputFile.c_str()) == TRUE;
#else
        bool Deleted = remove(PathOutputFile.c_str()) == 0;
#endif
        if (!Deleted) //Не удалось удалить файл
        {
            printf("Error deleting out file: %s\n", GetErrorString().c_str());
            return false;
        }
    }

    //Открываем выходной файл
    FILE *FileOut = fopen(PathOutputFile.c_str(), "wb");
    if (!FileOut)
    {
        printf("Error open out file: %s\n", GetErrorString().c_str());
        return false;
    }

    //Засекаем время
    auto TimeStart = std::chrono::steady_clock::now();
    unsigned long long FileOutSize = 0;

    size_t FilesCount = VectorFiles.size();
    for (size_t i = 0; i < FilesCount; ++i)
    {
        std::string FilePath = VectorFiles[i];
        printf("Reading file (%zd of %zd): %s\n", i + 1, FilesCount, FilePath.c_str());
        if (!ReadFile(FilePath, SeparatorIndex, FileOutSize, FileOut))
        {
            return false;
        }
    }
    fclose(FileOut); //Закрываем выходной файл
    printf("Complete with " CHRONO_FORMAT " msec. Files: %d. Size: %llu.\n",
           std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - TimeStart).count(),
           FilesCount, FileOutSize);
    return true;
}
//-----------------------------------------------------------------------------
bool ReadFile(const std::string &FilePath, size_t &SeparatorIndex, unsigned long long &FileOutSize, FILE *FileOut)
{
    FILE *File = fopen(FilePath.c_str(), "rb");
    if (!File) //Не удалось открыть файл
    {
        printf("Error open file: %s\n", GetErrorString().c_str());
        return false;
    }

    //Перемещаемся в конец файла
    int FSeek = fseek(File, 0L, SEEK_END);
    bool Result = FSeek == 0;
    if (Result) //Перемещение прошло успешно
    {
        //Получаем размер файла и возвращаемся в начало файла
        long FileSize = ftell(File);
        FileOutSize += FileSize;
        rewind(File);

        //Выделяем память под содержмое
        unsigned char *Data = (unsigned char *)malloc((size_t)FileSize + 1);
        Result = Data ? true : false;
        if (Result) //Память успешно выделена
        {
            long Readed = (long)fread(Data, sizeof(unsigned char), (size_t)FileSize, File);
            Result = Readed == FileSize;
            if (Result) //Файл прочитан успешно
            {
                //Копируем путь к файлу и отрезаем лишнее
                std::string Temp = FilePath;
                Temp.erase(0, SeparatorIndex);

                //Записываем заголовок файла
                fprintf(FileOut, "FileName=%s Size=%ld\n", Temp.c_str(), FileSize);

                //Записываем содержимое текущего файла в выходной файл
                Result = (long)fwrite(Data, sizeof(unsigned char), (size_t)FileSize, FileOut) == FileSize &&
                    (long)fwrite("\n", sizeof(unsigned char), 1, FileOut) == 1;
                if (Result) //Ошибка записи
                {
                    if (fflush(FileOut) != 0)
                    {
                        printf("No flushing data to out file: %s\n", GetErrorString().c_str());
                    }
                }
                else
                {
                    printf("Error write file content to out file: %s\n", GetErrorString().c_str());
                }
            }
            else //Ошибка чтения файла
            {
                printf("Error read file: %s\n", GetErrorString().c_str());
            }
            free(Data);
        }
        else //Не удалось выделить память
        {
            printf("Malloc error: %s\n", GetErrorString().c_str());
        }
    }
    else //Не удалось переместиться в конец файла
    {
        printf("Not seek to end file: %s\n", GetErrorString().c_str());
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
    ErrorString = strerror(errno);
#endif
    return ErrorString;
}
//-----------------------------------------------------------------------------
