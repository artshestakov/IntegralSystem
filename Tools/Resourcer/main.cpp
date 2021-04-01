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
bool CheckArgument(int argc, char** argv, std::string &DirPath); //�������� ���������
void PreparePath(std::string &DirPath); //���������� ����
bool RecursiveSearch(const std::string &DirPath, std::vector<std::string> &VectorFiles); //������ ����������� �����
bool ReadFiles(std::vector<std::string> &VectorFiles, size_t &SeparatorIndex); //������ ������
bool ReadFile(const std::string &FilePath, size_t &SeparatorIndex, FILE *FileOut); //������ �����
std::string GetErrorString(); //�������� ����� ��������� ������
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    //�������� ��������
    std::string DirPath;
    if (!CheckArgument(argc, argv, DirPath))
    {
        return EXIT_FAILURE;
    }

    PreparePath(DirPath);
    size_t SeparatorIndex = DirPath.size() - 1;

    //������ ����������
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
    //��������, ���� �� ������ ��������
    if (argc < 2) //��������� ��� - ������� ������, ������ � �������
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

    //�������� ������������� ������ ����
#ifdef WIN32
    bool PathExist = PathFileExists(DirPath.c_str()) == TRUE;
#else
    bool PathExist = access(DirPath.c_str(), F_OK) == 0;
#endif
    if (!PathExist) //��� ������ ���� - ������
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
        //���������� ��������� �� ������� � ���������� ��������
        if ((strcmp(FindData.cFileName, ".") == 0) || (strcmp(FindData.cFileName, "..") == 0))
        {
            continue;
        }
        
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //����������
        {
            if (!RecursiveSearch(DirPath + std::string(FindData.cFileName) + PATH_SEPARATOR, VectorFiles))
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
    if (Error != 0) //�� ������� ������� ����
    {
        LOG_E("Not open file: " + GetErrorString());
        return false;
    }

    //������������ � ����� �����
    int FSeek = fseek(File, 0L, SEEK_END);
    bool Result = FSeek == 0;
    if (Result) //����������� ������ �������
    {
        long FileSize = ftell(File);
        Result = FileSize != 1L;
        if (Result) //������ ����� �������
        {
            rewind(File); //������������ � ������ �����

            //�������� ������ ��� ���������
            unsigned char *Data = (unsigned char *)malloc(FileSize + 1);
            Result = Data ? true : false;
            if (Result) //������ ������� ��������
            {
                long Readed = fread_s(Data, FileSize, sizeof(unsigned char), FileSize, File);
                Result = Readed == FileSize;
                if (Result) //���� �������� �������
                {
                    //�������� ���� � ����� � �������� ������
                    std::string Temp = FilePath;
                    Temp.erase(0, SeparatorIndex);

                    fprintf_s(FileOut, "FileName=%s\n", Temp.c_str());
                    //���������� ���������� �������� ����� � �������� ����
                    Result = (long)fwrite(Data, sizeof(unsigned char), FileSize, FileOut) == FileSize &&
                        (long)fwrite("\n", sizeof(unsigned char), 1, FileOut) == 1;
                    if (Result) //������ ������
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
                else //������ ������ �����
                {
                    LOG_E("Error read file: " + GetErrorString());
                }
            }
            else //�� ������� �������� ������
            {
                LOG_E("Malloc error: " + GetErrorString());
            }
        }
        else //�� ������� �������� ������ �����
        {
            LOG_E("Not getting file size: " + GetErrorString());
        }
    }
    else //�� ������� ������������� � ����� �����
    {
        LOG_E("Not seek to end file: " + GetErrorString());
    }

    //��������� ���� � �������
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
