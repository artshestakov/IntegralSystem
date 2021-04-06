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
    //�������� ������� �����
    if (!ISAlgorithm::FileExist(FilePath))
    {
        ErrorString = ISAlgorithm::StringF("File \"%s\" not exist", FilePath.c_str());
        return false;
    }

    //��������� ����
    FILE *File = fopen(FilePath.c_str(), "rb");
    if (!File) //�� ������� ������� ����
    {
        ErrorString = ISAlgorithm::StringF("Not open file (%s): %s", FilePath.c_str(), ISAlgorithm::GetLastErrorS());
        return false;
    }

    //��������� � ����� �����
    if (fseek(File, 0L, SEEK_END) != 0)
    {
        ErrorString = ISAlgorithm::StringF("Error fseek: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }

    //�������� ������ �����
    Size = ftell(File);
    if (Size == -1) //������ ��� ��������� ������� �����
    {
        ErrorString = ISAlgorithm::StringF("Error ftell: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }
    rewind(File); //������������ � ������ �����

    //�������� ������ ��� �����
    Data = (unsigned char *)malloc(Size);
    if (!Data) //������ ��������� ������
    {
        ErrorString = ISAlgorithm::StringF("Error malloc: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }

    //������ ���������� �����
    if ((long)fread(Data, sizeof(unsigned char), Size, File) != Size)
    {
        ErrorString = ISAlgorithm::StringF("Error read file: %s", ISAlgorithm::GetLastErrorS());
        fclose(File);
        return false;
    }
    fclose(File); //��������� ����
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
