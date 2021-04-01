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
bool CheckArgument(int argc, char** argv, std::string &DirPath); //�������� ���������
void PreparePath(std::string &DirPath); //���������� ����
bool RecursiveSearch(const std::string &DirPath, std::vector<std::string> &VectorFiles); //������ ����������� �����
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

    //������ ����������
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
