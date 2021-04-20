#include "ISAlgorithm.h"
#include "ISConstants.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetClassName(const char *FunctionName)
{
    std::string Result(FunctionName);
    size_t Index = 0;

#ifndef WIN32 //Если работаем сейчас под Linux - исключаем имя типа
    Index = Result.find('('); //Ищем открывающуюся скобку
    if (Index != NPOS) //Если скобку нашли - удаляем все что после неё
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
ISErrorNumber ISAlgorithm::GetLastErrorN()
{
#ifdef WIN32
    return GetLastError();
#else
    return errno;
#endif
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GetLastErrorS()
{
    std::string ErrorString = STRING_UNKNOWN_ERROR;
#ifdef WIN32
    ISErrorNumber ErrorNumber = GetLastErrorN();
    if (ErrorNumber != 0) //Код ошибки валиден
    {
        LPSTR Buffer = nullptr;
        size_t MessageSize = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, ErrorNumber, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&Buffer, 0, NULL);
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

    //В реализации под Линукс нужно в начало вставлять разделитель пути
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
std::vector<ISFileInfo> ISAlgorithm::DirFiles(const std::string &DirPath, ISNamespace::DirFileSorting SortType, ISNamespace::SortingOrder SortOrder)
{
    std::string ErrorString;
    return DirFiles(DirPath, ErrorString, SortType, SortOrder);
}
//-----------------------------------------------------------------------------
std::vector<ISFileInfo> ISAlgorithm::DirFiles(const std::string &DirPath, std::string &ErrorString, ISNamespace::DirFileSorting SortType, ISNamespace::SortingOrder SortOrder)
{
    std::vector<ISFileInfo> Vector;
    if (DirExist(DirPath))
    {
        std::string DirPathTemp = DirPath;
        if (DirPathTemp.back() != PATH_SEPARATOR)
        {
            DirPathTemp.push_back(PATH_SEPARATOR);
        }
#ifdef WIN32
        DirPathTemp.push_back('*');

        WIN32_FIND_DATA FindData = { 0 };
        HANDLE Handle = FindFirstFile(DirPathTemp.c_str(), &FindData);
        if (Handle != INVALID_HANDLE_VALUE)
        {
            do
            {
                //Пропускаем указатели на текущий и родитеский каталоги
                if ((strcmp(FindData.cFileName, ".") == 0) || (strcmp(FindData.cFileName, "..") == 0))
                {
                    continue;
                }

                if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) //Файл
                {
                    ISFileInfo FileInfo;
                    FileInfo.Name = FindData.cFileName;
                    FileInfo.Path = DirPathTemp.substr(0, DirPathTemp.size() - 1) + FileInfo.Name;
                    FileInfo.Size = FindData.nFileSizeLow;

                    //Получаем дату изменения файла
                    SYSTEMTIME SystemTime = { 0 };
                    if (FileTimeToSystemTime(&FindData.ftLastWriteTime, &SystemTime) == TRUE)
                    {
                        FileInfo.DateTimeEdit.Date.Day = SystemTime.wDay;
                        FileInfo.DateTimeEdit.Date.Month = SystemTime.wMonth;
                        FileInfo.DateTimeEdit.Date.Year = SystemTime.wYear;
                        FileInfo.DateTimeEdit.Time.Hour = SystemTime.wHour;
                        FileInfo.DateTimeEdit.Time.Minute = SystemTime.wMinute;
                        FileInfo.DateTimeEdit.Time.Second = SystemTime.wSecond;
                        FileInfo.DateTimeEdit.Time.Milliseconds = SystemTime.wMilliseconds;
                    }

                    //Получаем временной сдвиг для даты изменения файла
                    TIME_ZONE_INFORMATION TimeZoneInfo;
                    if (GetTimeZoneInformationForYear(FileInfo.DateTimeEdit.Date.Year, NULL, &TimeZoneInfo) == TRUE)
                    {
                        FileInfo.DateTimeEdit.Time.Hour += (short)(TimeZoneInfo.Bias / TimeZoneInfo.DaylightBias);
                    }
                    Vector.emplace_back(FileInfo);
                }
            } while (FindNextFile(Handle, &FindData));
            FindClose(Handle);
        }
        else
        {
            ErrorString = ISAlgorithm::StringF("Error open path (%s): %s\n", DirPathTemp.c_str(), GetLastErrorS().c_str());
        }
#else
        DIR *Dir = opendir(DirPathTemp.c_str());
        if (Dir) //Директория успешно открыта
        {
            struct dirent *Entry = { 0 };

            while ((Entry = readdir(Dir)) != NULL)
            {
                //Пропускаем указатели на текущий и родитеский каталоги
                if ((strcmp(Entry->d_name, ".") == 0) || (strcmp(Entry->d_name, "..") == 0))
                {
                    continue;
                }

                ISFileInfo FileInfo;
                FileInfo.Name = Entry->d_name;
                FileInfo.Path = DirPathTemp + FileInfo.Name;

                //Получаем размер файла
                struct stat Stat;
                stat(FileInfo.Path.c_str(), &Stat);
                FileInfo.Size = Stat.st_size;

                //Получаем дату последнего изменения файла
                struct tm *TM = localtime(&Stat.st_mtim.tv_sec);
                FileInfo.DateTimeEdit.Date.Day = TM->tm_mday;
                FileInfo.DateTimeEdit.Date.Month = TM->tm_mon;
                FileInfo.DateTimeEdit.Date.Year = TM->tm_year;
                FileInfo.DateTimeEdit.Time.Hour = TM->tm_hour;
                FileInfo.DateTimeEdit.Time.Minute = TM->tm_min;
                FileInfo.DateTimeEdit.Time.Second = TM->tm_sec;
                Vector.emplace_back(FileInfo);
            }
            closedir(Dir);
        }
        else
        {
            ErrorString = GetLastErrorS();
        }
#endif
        if (!Vector.empty()) //Если файлы файлы - сортируем
        {
            switch (SortType)
            {
            case ISNamespace::DirFileSorting::EditDate:
                std::sort(Vector.begin(), Vector.end(), [SortOrder](const ISFileInfo &FileInfo1, const ISFileInfo &FileInfo2)
                {
                    return SortOrder == ISNamespace::SortingOrder::Ascending ?
                        FileInfo1.DateTimeEdit < FileInfo2.DateTimeEdit :
                        FileInfo1.DateTimeEdit > FileInfo2.DateTimeEdit;
                });
                break;

            case ISNamespace::DirFileSorting::Size:
                std::sort(Vector.begin(), Vector.end(), [SortOrder](const ISFileInfo &FileInfo1, const ISFileInfo &FileInfo2)
                {
                    return SortOrder == ISNamespace::SortingOrder::Ascending ?
                        FileInfo1.Size < FileInfo2.Size :
                        FileInfo1.Size > FileInfo2.Size;
                });
                break;

            case ISNamespace::DirFileSorting::Name:
                std::sort(Vector.begin(), Vector.end(), [SortOrder](const ISFileInfo &FileInfo1, const ISFileInfo &FileInfo2)
                {
                    return SortOrder == ISNamespace::SortingOrder::Ascending ?
                        FileInfo1.Name < FileInfo2.Name :
                        FileInfo1.Name > FileInfo2.Name;
                });
                break;

            default: //Сортировка не указана
                break;
            }
        }
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

#ifdef WIN32 //Для Windows удаляем расширение
        Pos = Path.find('.');
        if (Pos != NPOS) //Точка в названии есть - удаляем расширение
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
    char Buffer[HOST_NAME_MAX] = { 0 };
    if (gethostname(Buffer, HOST_NAME_MAX) == 0)
    {
        HostName = Buffer;
    }
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
    char Buffer[LOGIN_NAME_MAX] = { 0 };
    if (getlogin_r(Buffer, LOGIN_NAME_MAX) == 0)
    {
        UserName = Buffer;
    }
#endif
    return UserName;
}
//-----------------------------------------------------------------------------
ISVectorString ISAlgorithm::ParseArgs(int argc, char **argv)
{
    ISVectorString VectorString;
    if (argc > 1) //Если аргументов больше одного
    {
        VectorString.resize(argc - 1);

        //Начинаем цикл с единицы, чтобы не брать путь к исполняемому файлу приложения
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
    if (!String.empty()) //Если строка не пустая - анализируем
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
    //Получаем размер строки
    size_t Size = String.size();
    if (Size == 0) //Если строка пустая - выходим
    {
        return;
    }

    if (N >= Size) //Если кол-во символов больше либо равно размеру строки - очищаем её
    {
        String.clear();
    }
    else //Вырезаем N символов с конце строки
    {
        String.erase(Size - N, N);
    }
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::StringF(const char *Format, ...)
{
    //Вытаскиваем аргументы
    va_list Arguments;
    va_start(Arguments, Format);
    std::vector<char> Buffer(STRING_FORMAT_BUFFER_SIZE);

    while (true)
    {
        //Делаем копию аргументов заранее
        va_list Arguments2;
        va_copy(Arguments2, Arguments);

        //Форматируем
        size_t BufferSize = Buffer.size(),
            BufferSizeNew = 0; //Эта инициализация нужна для подавления предупреждения
        int Writed = std::vsnprintf(Buffer.data(), BufferSize, Format, Arguments2);
        if ((Writed >= 0) && (Writed < static_cast<int>(BufferSize))) //Форматирование удалось
        {
            va_end(Arguments);
            va_end(Arguments2);
            return std::string(Buffer.data());
        }
        //При форматировании что-то пошло не так - увеличиваем размер буфера
        BufferSizeNew = Writed < 0 ? (BufferSizeNew * 2) : (Writed + 1);
        Buffer.clear();
        Buffer.resize(BufferSizeNew);
        va_end(Arguments2);
    }
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::StringToMD5(const std::string &String)
{
    std::string Result(MD5_RESULT_SIZE, CHAR_NULL_TERM);
    unsigned char Hash[MD5_SIZE] = { 0 };
    bool IsOk = false;
#ifdef WIN32
    HCRYPTPROV HCryptoProv = 0;
    if (CryptAcquireContext(&HCryptoProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == TRUE)
    {
        HCRYPTHASH CryptoHash = 0;
        if (CryptCreateHash(HCryptoProv, CALG_MD5, 0, 0, &CryptoHash) == TRUE)
        {
            if (CryptHashData(CryptoHash, (unsigned char *)String.c_str(), (DWORD)String.size(), 0) == TRUE)
            {
                unsigned long MD5Size = MD5_SIZE;
                if (CryptGetHashParam(CryptoHash, HP_HASHVAL, Hash, &MD5Size, 0) == TRUE)
                {
                    IsOk = true;
                }
            }
            CryptDestroyHash(CryptoHash);
        }
        CryptReleaseContext(HCryptoProv, 0);
    }
#else
    MD5_CTX Context;
    if (MD5_Init(&Context) == 1)
    {
        if (MD5_Update(&Context, String.c_str(), String.size()) == 1)
        {
            if (MD5_Final(Hash, &Context) == 1)
            {
                IsOk = true;
            }
        }
    }
#endif
    if (IsOk)
    {
        size_t Index = 0;
        for (size_t i = 0; i < MD5_SIZE; ++i, ++Index)
        {
            Result[Index] = MD5_DIGITS[Hash[i] >> 4];
            Result[++Index] = MD5_DIGITS[Hash[i] & 0xF];
        }
    }
    return Result;
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::GenerateUuidStandart()
{
    std::string StringUID(UUID_STANDART_SIZE, CHAR_NULL_TERM);
#ifdef WIN32
    GUID UID = { 0 };
    HRESULT Result = CoCreateGuid(&UID); //Генерируем идентификатор
    if (Result == S_OK) //Генерация прошла успешно
    {
        unsigned char *Char = { 0 };
        if (UuidToString(&UID, &Char) == RPC_S_OK) //Преобразовываем в строку
        {
            //Заполняем строку
            for (size_t i = 0; i < UUID_STANDART_SIZE; ++i)
            {
                StringUID[i] = Char[i];
            }
        }
    }
#else
    //Генерируем идентификатор
    uuid_t UUID = { 0 };
    uuid_generate(UUID);

    //Переводим его в строку
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
bool ISAlgorithm::GenerateSalt(std::string &Salt, std::string &ErrorString)
{
    //Объявляем результирующую строку и буфер
    unsigned char Buffer[CARAT_SALT_SIZE] = { 0 };
#ifdef WIN32 //Формирование соли под Windows
    HCRYPTPROV CryptoProvider = 0;
    bool Result = CryptAcquireContext(&CryptoProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT) == TRUE;
    if (Result) //Контекст создан успешно - формируем соль
    {
        Result = CryptGenRandom(CryptoProvider, CARAT_SALT_SIZE, Buffer) == TRUE;
        if (Result) //Генерация прошла успешно - освобождаем контекст
        {
            CryptReleaseContext(CryptoProvider, 0);
        }
        else //Ошибка генерации
        {
            ErrorString = GetLastErrorS();
        }
    }
    else //Не удалось создать контекст
    {
        ErrorString = GetLastErrorS();
    }
#else //Формирование соли под Linux
    FILE *FileDevice = fopen("/dev/random", "r");
    bool Result = FileDevice ? true : false;
    if (Result) //Устройство удалось открыть - читаем и закрываем устройство
    {
        Result = fread(&Buffer[0], sizeof(char), CARAT_SALT_SIZE, FileDevice) == CARAT_SALT_SIZE;
        fclose(FileDevice);
    }
    else
    {
        ErrorString = GetLastErrorS();
    }
#endif
    if (Result) //Если все хорошо - формируем соль в HEX
    {
        std::stringstream StringStream;
        for (unsigned long i = 0; i < CARAT_SALT_SIZE; ++i) //Обходим буфер с солью
        {
            StringStream << std::setfill('0') << std::setw(2) << std::hex << (0xFF & (unsigned int)Buffer[i]);
        }
        Salt = StringStream.str();
        ISAlgorithm::StringToUpper(Salt);
    }
    return Result;
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
std::string ISAlgorithm::Base64Encode(const std::string &String, std::string &ErrorString)
{
    return Base64Encode((unsigned char *)String.c_str(), String.size(), ErrorString);
}
//-----------------------------------------------------------------------------
std::string ISAlgorithm::Base64Encode(unsigned char *Data, size_t Size, std::string &ErrorString)
{
    std::string Result;
#ifdef WIN32
    DWORD ResultSize = 0;
    if (CryptBinaryToStringA(Data, (DWORD)Size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &ResultSize) == TRUE)
    {
        char *BufferResult = (char *)malloc(sizeof(char) * ResultSize);
        if (BufferResult)
        {
            if (CryptBinaryToStringA(Data, (DWORD)Size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, BufferResult, &ResultSize))
            {
                Result = &BufferResult[0];
            }
            else
            {
                ErrorString = GetLastErrorS();
            }
            free(BufferResult);
        }
        else //Ошибка выделения памяти
        {
            ErrorString = "Error malloc";
        }
    }
    else
    {
        ErrorString = GetLastErrorS();
    }
#else
    BIO *Bio = BIO_new(BIO_s_mem()), *B64 = BIO_new(BIO_f_base64());
    Bio = BIO_push(B64, Bio);

    BIO_set_flags(Bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(Bio, Data, Size);
    BIO_flush(Bio);

    BUF_MEM *Buffer = nullptr;
    BIO_get_mem_ptr(Bio, &Buffer);
    BIO_set_close(Bio, BIO_NOCLOSE);
    BIO_free_all(Bio);
    Result = &(*Buffer).data[0];
    if (Result.empty())
    {
       ErrorString = GetLastErrorS();
    }
#endif
    return Result;
}
//-----------------------------------------------------------------------------
unsigned char* ISAlgorithm::ReadFile(const char *FilePath, const char *Mode, long &FileSize)
{
    std::string ErrorString;
    return ReadFile(FilePath, Mode, FileSize, ErrorString);
}
//-----------------------------------------------------------------------------
unsigned char* ISAlgorithm::ReadFile(const char *FilePath, const char *Mode, long &FileSize, std::string &ErrorString)
{
    //Открываем файл
    FILE *File = fopen(FilePath, Mode);
    if (!File) //Не удалось открыть файл
    {
        ErrorString = ISAlgorithm::StringF("Error open file: %s", ISAlgorithm::GetLastErrorS().c_str());
        return nullptr;
    }

    //Смещаемся в конец файла
    if (fseek(File, 0L, SEEK_END) != 0)
    {
        ErrorString = ISAlgorithm::StringF("FSeek: %s", ISAlgorithm::GetLastErrorS().c_str());
        fclose(File);
        return nullptr;
    }

    //Получаем размер файла
    FileSize = ftell(File);
    if (FileSize == -1) //Ошибка при получении размера файла
    {
        ErrorString = ISAlgorithm::StringF("Error ftell: %s", ISAlgorithm::GetLastErrorS().c_str());
        fclose(File);
        return nullptr;
    }
    rewind(File); //Возвращаемся в начало файла

    //Выделяем память под буфер
    unsigned char *FileData = (unsigned char *)malloc(FileSize);
    if (!FileData) //Ошибка выделения памяти
    {
        ErrorString = ISAlgorithm::StringF("Error malloc: %s", ISAlgorithm::GetLastErrorS().c_str());
        fclose(File);
        return nullptr;
    }

    //Читаем содержимое файла
    if ((long)fread(FileData, sizeof(unsigned char), FileSize, File) != FileSize)
    {
        ErrorString = ISAlgorithm::StringF("Error read file: %s", ISAlgorithm::GetLastErrorS().c_str());
        fclose(File);
        free(FileData);
        FileData = nullptr;
        return nullptr;
    }
    fclose(File); //Закрываем файл
    return FileData;
}
//-----------------------------------------------------------------------------
