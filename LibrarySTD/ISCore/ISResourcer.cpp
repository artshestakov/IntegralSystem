#include "ISResourcer.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISResourcer::ISResourcer()
    : ErrorString(STRING_NO_ERROR),
    Data(nullptr),
    SizeData(0),
    FileCount(0)
{

}
//-----------------------------------------------------------------------------
ISResourcer::~ISResourcer()
{
    Free();
}
//-----------------------------------------------------------------------------
std::string ISResourcer::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISResourcer::LoadFile(const std::string &FilePath)
{
    //Проверим наличие файла
    if (!ISAlgorithm::FileExist(FilePath))
    {
        ErrorString = ISAlgorithm::StringF("File \"%s\" not exist", FilePath.c_str());
        return false;
    }

    //Открываем файл
    FILE *File = fopen(FilePath.c_str(), "rb");
    if (!File) //Не удалось открыть файл
    {
        ErrorString = ISAlgorithm::StringF("Not open file (%s): %s", FilePath.c_str(), ISAlgorithm::GetLastErrorS());
        return false;
    }

    //Смещаемся в конец файла
    if (fseek(File, 0L, SEEK_END) != 0)
    {
        ErrorString = ISAlgorithm::StringF("Error fseek: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }

    //Получаем размер файла
    SizeData = ftell(File);
    if (SizeData == -1) //Ошибка при получении размера файла
    {
        ErrorString = ISAlgorithm::StringF("Error ftell: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }
    rewind(File); //Возвращаемся в начало файла

    //Выделяем память под буфер
    Data = (char *)malloc(SizeData);
    if (!Data) //Ошибка выделения памяти
    {
        ErrorString = ISAlgorithm::StringF("Error malloc: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }

    //Читаем содержимое файла
    if ((long)fread(Data, sizeof(unsigned char), SizeData, File) != SizeData)
    {
        ErrorString = ISAlgorithm::StringF("Error read file: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }
    fclose(File); //Закрываем файл
    
    //Парсим файл и освобождаем память
    bool Result = ParseFile();
    if (Data)
    {
        free(Data);
        Data = nullptr;
    }
    return Result;
}
//-----------------------------------------------------------------------------
void ISResourcer::Free()
{
    while (!Files.empty())
    {
        free(ISAlgorithm::VectorTakeBack(Files).Data);
    }
}
//-----------------------------------------------------------------------------
const char* ISResourcer::GetFile(const std::string &FileName)
{
    unsigned long FileSize = 0;
    return GetFile(FileName, FileSize);
}
//-----------------------------------------------------------------------------
const char* ISResourcer::GetFile(const std::string &FileName, unsigned long &FileSize)
{
    ISResourceFile ResourceFile;
    for (size_t i = 0; i < FileCount; ++i)
    {
        ResourceFile = Files[i];
        if (ResourceFile.Name == FileName)
        {
            FileSize = ResourceFile.Size;
            return ResourceFile.Data;
        }
    }
    return nullptr;
}
//-----------------------------------------------------------------------------
bool ISResourcer::ParseFile()
{
    size_t PosHeaderBegin = 0;
    for (long i = 0; i < SizeData; ++i)
    {
        if (Data[i] == '\n') //Конец заголовка файла
        {
            std::string FileName;
            unsigned long FileSize = 0;

            //Парсим заголовок файла
            if (!ParseHeader(std::string(Data + PosHeaderBegin, Data + i), FileName, FileSize))
            {
                return false;
            }

            //Выделяем память под файл
            char *FileData = (char *)malloc(FileSize);
            if (!FileData) //Ошибка выделения памяти
            {
                ErrorString = ISAlgorithm::StringF("not allocate memory %d", FileSize);
                return false;
            }

            //Копируем контент файла в выделенную память и добавляем в список файлов
            memcpy(FileData, Data + i + 1, FileSize);
            FileData[FileSize] = '\0';
            Files.emplace_back(ISResourceFile{ FileName, FileSize, FileData });
            PosHeaderBegin += FileSize + 2 + (i - PosHeaderBegin);
            i += ++FileSize;
            ++FileCount;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISResourcer::ParseHeader(const std::string &StringHeader, std::string &FileName, unsigned long &FileSize)
{
    //Получаем секции параметров
    ISVectorString Vector = ISAlgorithm::StringSplit(StringHeader, ' ');
    if (Vector.size() != 2)
    {
        ErrorString = "Invalid header: " + StringHeader;
        return false;
    }

    //Получаем имя файла
    std::string Temp = Vector.front();
    size_t Pos = Temp.find('=');
    if (Pos == NPOS)
    {
        ErrorString = "no getting file name";
        return false;
    }
    Temp.erase(0, ++Pos);
    FileName = Temp;

    //Получаем размер файла
    Temp = Vector.back();
    Pos = Temp.find('=');
    if (Pos == NPOS)
    {
        ErrorString = "no getting file size";
        return false;
    }
    Temp.erase(0, ++Pos);
    FileSize = std::stoul(Temp);
    return true;
}
//-----------------------------------------------------------------------------
