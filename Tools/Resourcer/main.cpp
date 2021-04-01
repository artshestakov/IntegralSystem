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
const char OUTPUT_FILE_NAME[] = "Resources.bin";
const size_t BUFFER_ERROR_SIZE = 1024;
//-----------------------------------------------------------------------------
bool CheckArgument(int argc, char** argv, std::string &DirPath); //�������� ���������
void PreparePath(std::string &DirPath); //���������� ����
bool GetFilesPath(const std::string &DirPath, std::vector<std::string> &VectorFiles); //������ ����������� �����
bool ReadFiles(std::vector<std::string> &VectorFiles, size_t &SeparatorIndex); //������ ������
bool ReadFile(const std::string &FilePath, size_t &SeparatorIndex, unsigned long long &FileOutSize, FILE *FileOut); //������ �����
std::string GetErrorString(); //�������� ����� ��������� ������
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    //��������� �������� �� ������ ������
    setlocale(LC_ALL, "Russian");

    std::string DirPath;

    //�������� ��������
    if (!CheckArgument(argc, argv, DirPath))
    {
        return EXIT_FAILURE;
    }

    PreparePath(DirPath);
    size_t SeparatorIndex = DirPath.size();

    //������ ����������
    printf("Getting files list...\n");
    std::vector<std::string> VectorFiles;
    if (!GetFilesPath(DirPath.c_str(), VectorFiles))
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
    //��������, ���� �� ������ ��������
    if (argc < 2) //��������� ��� - ������� ������, ������ � �������
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

    //�������� ������������� ������ ����
#ifdef WIN32
    bool PathExist = PathFileExists(DirPath.c_str()) == TRUE;
#else
    bool PathExist = access(DirPath.c_str(), F_OK) == 0;
#endif
    if (!PathExist) //��� ������ ���� - ������
    {
        printf("Path \"%s\" not exist\n", DirPath.c_str());
        return false;
    }

#ifdef WIN32
    DWORD Attributes = GetFileAttributes(DirPath.c_str());
    bool PathIsDir = (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
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
        //���������� ��������� �� ������� � ���������� ��������
        if ((strcmp(FindData.cFileName, ".") == 0) || (strcmp(FindData.cFileName, "..") == 0))
        {
            continue;
        }
        
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //����������
        {
            if (!GetFilesPath(DirPath + std::string(FindData.cFileName) + PATH_SEPARATOR, VectorFiles))
            {
                return false;
            }
        }
        else //����
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
    //�������� ������� ��������� �����
#ifdef WIN32
    bool IsExist = PathFileExists(OUTPUT_FILE_NAME) == TRUE;
#else
    bool IsExist = access(OUTPUT_FILE_NAME, F_OK) == 0;
#endif
    if (IsExist) //���� ���� ���������� - ������� ������� ���
    {
#ifdef WIN32
        bool Deleted = DeleteFile(OUTPUT_FILE_NAME) == TRUE;
#else
        bool Deleted = remove(OUTPUT_FILE_NAME) == 0;
#endif
        if (!Deleted) //�� ������� ������� ����
        {
            printf("Error deleting out file: %s\n", GetErrorString().c_str());
            return false;
        }
    }

    FILE *FileOut = nullptr;
    errno_t Error = fopen_s(&FileOut, OUTPUT_FILE_NAME, "wb");
    if (Error != 0)
    {
        printf("Error open out file: %s\n", GetErrorString().c_str());
        return false;
    }

    //�������� �����
    auto TimeStart = std::chrono::steady_clock::now();
    unsigned long long FileOutSize = 0;

    for (size_t i = 0, c = VectorFiles.size(); i < c; ++i)
    {
        std::string FilePath = VectorFiles[i];
        printf("Reading file (%zd of %zd): %s\n", i + 1, c, FilePath.c_str());
        if (!ReadFile(FilePath, SeparatorIndex, FileOutSize, FileOut))
        {
            return false;
        }
    }
    fclose(FileOut);
    printf("Complete with %llu msec. Size: %llu\n", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - TimeStart).count(), FileOutSize);
    return true;
}
//-----------------------------------------------------------------------------
bool ReadFile(const std::string &FilePath, size_t &SeparatorIndex, unsigned long long &FileOutSize, FILE *FileOut)
{
    FILE *File = nullptr;
    errno_t Error = fopen_s(&File, FilePath.c_str(), "rb");
    if (Error != 0) //�� ������� ������� ����
    {
        printf("Error open file: %s\n", GetErrorString().c_str());
        return false;
    }

    //������������ � ����� �����
    int FSeek = fseek(File, 0L, SEEK_END);
    bool Result = FSeek == 0;
    if (Result) //����������� ������ �������
    {
        //�������� ������ ����� � ������������ � ������ �����
        long FileSize = ftell(File);
        FileOutSize += FileSize;
        rewind(File);

        //�������� ������ ��� ���������
        unsigned char *Data = (unsigned char *)malloc(FileSize + 1);
        Result = Data ? true : false;
        if (Result) //������ ������� ��������
        {
            long Readed = (long)fread_s(Data, FileSize, sizeof(unsigned char), FileSize, File);
            Result = Readed == FileSize;
            if (Result) //���� �������� �������
            {
                //�������� ���� � ����� � �������� ������
                std::string Temp = FilePath;
                Temp.erase(0, SeparatorIndex);

                //���������� ��������� �����
                fprintf_s(FileOut, "FileName=%s Size=%ld\n", Temp.c_str(), FileSize);

                //���������� ���������� �������� ����� � �������� ����
                Result = (long)fwrite(Data, sizeof(unsigned char), FileSize, FileOut) == FileSize &&
                    (long)fwrite("\n", sizeof(unsigned char), 1, FileOut) == 1;
                if (Result) //������ ������
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
            else //������ ������ �����
            {
                printf("Error read file: %s\n", GetErrorString().c_str());
            }
            free(Data);
        }
        else //�� ������� �������� ������
        {
            printf("Malloc error\n");
        }
    }
    else //�� ������� ������������� � ����� �����
    {
        printf("Not seek to end file: %s\n", GetErrorString().c_str());
    }

    //��������� ���� � �������
    fclose(File);
    return Result;
}
//-----------------------------------------------------------------------------
std::string GetErrorString()
{
    std::string ErrorString = "Unknown error";
#ifdef WIN32
    DWORD ErrorID = GetLastError();
    if (ErrorID != 0) //��� ������ �������
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
