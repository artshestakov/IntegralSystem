#include "ISResourcer.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISResourcer::ISResourcer()
    : ErrorString(STRING_NO_ERROR),
    Data(nullptr),
    Size(0)
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
    Size = ftell(File);
    if (Size == -1) //Ошибка при получении размера файла
    {
        ErrorString = ISAlgorithm::StringF("Error ftell: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }
    rewind(File); //Возвращаемся в начало файла

    //Выделяем память под буфер
    Data = (unsigned char *)malloc(Size);
    if (!Data) //Ошибка выделения памяти
    {
        ErrorString = ISAlgorithm::StringF("Error malloc: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }

    //Читаем содержимое файла
    if ((long)fread(Data, sizeof(unsigned char), Size, File) != Size)
    {
        ErrorString = ISAlgorithm::StringF("Error read file: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }
    fclose(File); //Закрываем файл
    return Parse();
}
//-----------------------------------------------------------------------------
void ISResourcer::Free()
{
    if (Data)
    {
        free(Data);
    }
}
//-----------------------------------------------------------------------------
bool ISResourcer::Parse()
{
    size_t Pos = 0;



    return true;
}
//-----------------------------------------------------------------------------
