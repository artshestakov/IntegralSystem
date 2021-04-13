#include "ISAlgorithm.h"
#include "ISConstants.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetClassName(const char *FunctionName)
{
    std::string Result(FunctionName);
    size_t Index = 0;

#ifndef WIN32 //���� �������� ������ ��� Linux - ��������� ��� ����
    Index = Result.find('('); //���� ������������� ������
    if (Index != NPOS) //���� ������ ����� - ������� ��� ��� ����� ��
    {
        Result.erase(Index, Result.size() - Index);
    }

    while ((Index = Result.find(' ')) != NPOS)
    {
        Result.erase(0, ++Index);
    }
#endif

    Index = Result.find(':');
    if (Index != NPOS)
    {
        Result.erase(Index, Result.size() - Index);
    }
    return Result;
}
//-----------------------------------------------------------------------------
ISTimePoint ISAlgorithm::GetTick()
{
    return std::chrono::steady_clock::now();
}
//-----------------------------------------------------------------------------
ISUInt64 ISAlgorithm::GetTickDiff(const ISTimePoint &T1, const ISTimePoint &T2)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(T1 - T2).count();
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetLastErrorS()
{
    std::string ErrorString = STRING_UNKNOWN_ERROR;
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
    ErrorString = strerror(errno);
#endif
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::ConsoleSetEncoding(unsigned int CodePage, std::string &ErrorString)
{
#ifdef WIN32
    if (SetConsoleOutputCP(CodePage) == FALSE) //�� ������� ���������� ������� ��������
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        return false;
    }
    return true;
#else //���������� ��� Linux �� ����������
    IS_UNUSED(CodePage);
    IS_UNUSED(ErrorString);
    return true;
#endif
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::DirExist(const std::string &DirPath)
{
#ifdef WIN32
    DWORD Attributes = GetFileAttributes(DirPath.c_str());
    return (Attributes != INVALID_FILE_ATTRIBUTES && (Attributes & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat Stat;
    return stat(DirPath.c_str(), &Stat) == 0 && S_ISDIR(Stat.st_mode);
#endif
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::DirCreate(const std::string &DirPath)
{
    std::string ErrorString = STRING_NO_ERROR;
    return DirCreate(DirPath, ErrorString);
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::DirCreate(const std::string &DirPath, std::string &ErrorString)
{
    ISVectorString VectorString = StringSplit(DirPath, PATH_SEPARATOR);
    std::string TempPath;

    //� ���������� ��� ������ ����� � ������ ��������� ����������� ����
#ifndef WIN32
    TempPath.push_back(PATH_SEPARATOR);
#endif

    for (const std::string &String : VectorString)
    {
        TempPath += String + PATH_SEPARATOR;
        if (!DirExist(TempPath))
        {
#ifdef WIN32
            if (CreateDirectory(TempPath.c_str(), NULL) != TRUE)
#else
            if (mkdir(TempPath.c_str(), 0777) != 0)
#endif
            {
                ErrorString = GetLastErrorS();
                return false;
            }
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
ISVectorString ISAlgorithm::DirFiles(const std::string &DirPath, std::string &ErrorString)
{
    ISVectorString Vector;
    if (DirExist(DirPath))
    {
#ifdef WIN32
        std::string DirPathTemp = DirPath;
        if (DirPathTemp.back() != PATH_SEPARATOR)
        {
            DirPathTemp.push_back(PATH_SEPARATOR);
        }
        DirPathTemp.push_back('*');

        WIN32_FIND_DATA FindData = { 0 };
        HANDLE Handle = FindFirstFile(DirPathTemp.c_str(), &FindData);
        if (Handle != INVALID_HANDLE_VALUE)
        {
            do
            {
                //���������� ��������� �� ������� � ���������� ��������
                if ((strcmp(FindData.cFileName, ".") == 0) || (strcmp(FindData.cFileName, "..") == 0))
                {
                    continue;
                }

                if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) //����
                {
                    Vector.emplace_back(FindData.cFileName);
                }
            } while (FindNextFile(Handle, &FindData));
            FindClose(Handle);
        }
        else
        {
            ErrorString = ISAlgorithm::StringF("Error open path (%s): %s\n", DirPathTemp.c_str(), GetLastErrorS().c_str());
        }
#else
        IS_UNUSED(ErrorString);
        IS_ASSERT(false, "not support");
#endif
    }
    return Vector;
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::FileExist(const std::string &FilePath)
{
#ifdef WIN32
    return PathFileExists(FilePath.c_str()) == TRUE;
#else
    return access(FilePath.c_str(), F_OK) == 0;
#endif
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetApplicationPath()
{
    std::string Path;
    char Buffer[MAX_PATH] = { 0 };
#ifdef WIN32
    if (GetModuleFileName(GetModuleHandle(NULL), Buffer, sizeof(Buffer)) > 0)
    {
        Path = Buffer;
    }
#else
    char Temp[20] = { 0 };
    sprintf(Temp, "/proc/%d/exe", CURRENT_PID());
    size_t Size = readlink(Temp, Buffer, 1024);
    Path = std::string(Buffer, Size);
#endif
    return Path;
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::FileDelete(const std::string &FilePath)
{
    std::string ErrorString;
    return FileDelete(FilePath, ErrorString);
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::FileDelete(const std::string &FilePath, std::string &ErrorString)
{
#ifdef WIN32
    if (DeleteFile(FilePath.c_str()) == FALSE)
    {
        ErrorString = GetLastErrorS();
        return false;
    }
#else
    if (remove(FilePath.c_str()) != 0)
    {
        ErrorString = GetLastErrorS();
        return false;
    }
#endif
    return true;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetApplicationDir()
{
    std::string Path = ISAlgorithm::GetApplicationPath();
    if (!Path.empty())
    {
        size_t Pos = Path.rfind(PATH_SEPARATOR);
        if (Pos != NPOS)
        {
            Path.erase(Pos, Path.size() - Pos);
        }
    }
    return Path;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetApplicationName()
{
    std::string Path = ISAlgorithm::GetApplicationPath();
    if (!Path.empty())
    {
        size_t Pos = Path.rfind(PATH_SEPARATOR);
        if (Pos != NPOS)
        {
            Path.erase(0, ++Pos);
        }

#ifdef WIN32 //��� Windows ������� ����������
        Pos = Path.find('.');
        if (Pos != NPOS) //����� � �������� ���� - ������� ����������
        {
            Path.erase(Pos, Path.size() - Pos);
        }
#endif
    }
    return Path;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetHostName()
{
    std::string HostName;
#ifdef WIN32
    char Buffer[32] = { 0 };
    DWORD Size = sizeof(Buffer);
    if (GetComputerNameEx(ComputerNameNetBIOS, Buffer, &Size) == TRUE)
    {
        HostName = Buffer;
    }
#else
    IS_ASSERT(false, "not support");
#endif
    return HostName;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetUserName()
{
    std::string UserName;
#ifdef WIN32
    char Buffer[32] = { 0 };
    DWORD Size = sizeof(Buffer);
    if (::GetUserName(Buffer, &Size) == TRUE)
    {
        UserName = Buffer;
    }
#else
    IS_ASSERT(false, "not support");
#endif
    return UserName;
}
//-----------------------------------------------------------------------------
ISVectorString ISAlgorithm::ParseArgs(int argc, char **argv)
{
    ISVectorString VectorString;
    if (argc > 1) //���� ���������� ������ ������
    {
        VectorString.resize(argc - 1);

        //�������� ���� � �������, ����� �� ����� ���� � ������������ ����� ����������
        for (int i = 1; i < argc; ++i)
        {
            VectorString[i - 1] = argv[i];
        }
    }
    return VectorString;
}
//-----------------------------------------------------------------------------
ISVectorString ISAlgorithm::StringSplit(const std::string &String, char Separator)
{
    ISVectorString VectorString;
    if (!String.empty()) //���� ������ �� ������ - �����������
    {
        size_t Pos = 0, LastPos = 0;
        while ((Pos = String.find(Separator, LastPos)) != NPOS)
        {
            if (Pos != 0)
            {
                VectorString.emplace_back(String.substr(LastPos, Pos - LastPos));
                LastPos = ++Pos;
            }
            else
            {
                ++LastPos;
            }
        }

        if (Pos == NPOS)
        {
            size_t Size = String.size();
            if (LastPos < Size)
            {
                VectorString.emplace_back(String.substr(LastPos, Size - LastPos));
            }
        }
    }
    return VectorString;
}
//-----------------------------------------------------------------------------
bool ISAlgorithm::StringIsNumber(const std::string &String)
{
    for (size_t i = 0, c = String.size(); i < c; ++i)
    {
        if (!std::isdigit(String[i]))
        {
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
void ISAlgorithm::StringToLower(std::string &String)
{
    if (!String.empty())
    {
        std::transform(String.begin(), String.end(), String.begin(), tolower);
    }
}
//-----------------------------------------------------------------------------
void ISAlgorithm::StringToUpper(std::string &String)
{
    if (!String.empty())
    {
        std::transform(String.begin(), String.end(), String.begin(), toupper);
    }
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::StringRight(const std::string &String, size_t N)
{
    std::string Result;
    if (!String.empty())
    {
        Result = String.substr(String.size() - N, N);
    }
    return Result;
}
//-----------------------------------------------------------------------------
void ISAlgorithm::StringChop(std::string &String, size_t N)
{
    //�������� ������ ������
    size_t Size = String.size();
    if (Size == 0) //���� ������ ������ - �������
    {
        return;
    }

    if (N >= Size) //���� ���-�� �������� ������ ���� ����� ������� ������ - ������� �
    {
        String.clear();
    }
    else //�������� N �������� � ����� ������
    {
        String.erase(Size - N, N);
    }
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::StringF(const char *Format, ...)
{
    //����������� ���������
    va_list Arguments;
    va_start(Arguments, Format);
    std::vector<char> Buffer(STRING_FORMAT_BUFFER_SIZE);

    while (true)
    {
        //������ ����� ���������� �������
        va_list Arguments2;
        va_copy(Arguments2, Arguments);

        //�����������
        size_t BufferSize = Buffer.size(),
            BufferSizeNew = 0; //��� ������������� ����� ��� ���������� ��������������
        int Writed = std::vsnprintf(Buffer.data(), BufferSize, Format, Arguments2);
        if ((Writed >= 0) && (Writed < static_cast<int>(BufferSize))) //�������������� �������
        {
            va_end(Arguments);
            va_end(Arguments2);
            return std::string(Buffer.data());
        }
        //��� �������������� ���-�� ����� �� ��� - ����������� ������ ������
        BufferSizeNew = Writed < 0 ? (BufferSizeNew * 2) : (Writed + 1);
        Buffer.clear();
        Buffer.resize(BufferSizeNew);
        va_end(Arguments2);
    }
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GenerateUuidStandart()
{
    std::string StringUID(UUID_STANDART_SIZE, CHAR_NULL_TERM);
#ifdef WIN32
    GUID UID = { 0 };
    HRESULT Result = CoCreateGuid(&UID); //���������� �������������
    if (Result == S_OK) //��������� ������ �������
    {
        unsigned char *Char = { 0 };
        if (UuidToString(&UID, &Char) == RPC_S_OK) //��������������� � ������
        {
            //��������� ������
            for (size_t i = 0; i < UUID_STANDART_SIZE; ++i)
            {
                StringUID[i] = Char[i];
            }
        }
    }
#else
    //���������� �������������
    uuid_t UUID = { 0 };
    uuid_generate(UUID);

    //��������� ��� � ������
    char Char[UUID_STANDART_SIZE] = { 0 };
    uuid_unparse(UUID, Char);
    StringUID = Char;
#endif
    return StringUID;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GenerateUuid()
{
    std::string UID = ISAlgorithm::GenerateUuidStandart();
    std::transform(UID.begin(), UID.end(), UID.begin(), toupper);
    return '{' + UID + '}';
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GenerateUuidLite()
{
    std::string UID = ISAlgorithm::GenerateUuidStandart();
    std::transform(UID.begin(), UID.end(), UID.begin(), tolower);
    auto Begin = UID.begin();
    for (size_t i = UID.size() - 1; i > 0; --i)
    {
        if (UID[i] == '-')
        {
            UID.erase(Begin + i);
        }
    }
    return UID;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::SaltPassword(const std::string &HashPassword, const std::string &Salt)
{
    std::string HashResult;
    for (int i = 0; i < (int)CARAT_HASH_SIZE; ++i)
    {
        HashResult.push_back(HashPassword[i]);
        HashResult.push_back(Salt[i]);
    }
    std::reverse(HashResult.begin(), HashResult.end());
    return HashResult;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::MD5(const std::string &String)
{
    std::string Result(MD5_RESULT_SIZE, CHAR_NULL_TERM);
#ifdef WIN32
    HCRYPTPROV HCryptoProv = 0;
    if (CryptAcquireContext(&HCryptoProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == TRUE)
    {
        HCRYPTHASH CryptoHash = 0;
        if (CryptCreateHash(HCryptoProv, CALG_MD5, 0, 0, &CryptoHash) == TRUE)
        {
            if (CryptHashData(CryptoHash, (unsigned char *)String.c_str(), (DWORD)String.size(), 0) == TRUE)
            {
                unsigned char Hash[MD5_SIZE] = { 0 };
                unsigned long MD5Size = MD5_SIZE;
                if (CryptGetHashParam(CryptoHash, HP_HASHVAL, Hash, &MD5Size, 0) == TRUE)
                {
                    size_t Index = 0;
                    for (size_t i = 0; i < MD5_SIZE; ++i, ++Index)
                    {
                        Result[Index] = MD5_DIGITS[Hash[i] >> 4];
                        Result[++Index] = MD5_DIGITS[Hash[i] & 0xF];
                    }
                }
            }
            CryptDestroyHash(CryptoHash);
        }
        CryptReleaseContext(HCryptoProv, 0);
    }
#else
    IS_UNUSED(String);
    IS_ASSERT(false, "not support");
#endif
    return Result;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::Base64Encode(const std::string &String)
{
    std::string Result;
#ifdef WIN32
    const unsigned char *Buffer = (const unsigned char *)String.c_str();
    DWORD BufferSize = (DWORD)String.size();
    DWORD ResultSize = 0;
    if (CryptBinaryToStringA(Buffer, BufferSize, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &ResultSize) == TRUE)
    {
        char *BufferResult = (char *)malloc(sizeof(char) * ResultSize);
        if (BufferResult && CryptBinaryToStringA(Buffer, BufferSize, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, BufferResult, &ResultSize))
        {
            Result = &BufferResult[0];
            free(BufferResult);
        }
    }
#else
    IS_UNUSED(String);
    IS_ASSERT(false, "not support");
#endif
    return Result;
}
//-----------------------------------------------------------------------------
