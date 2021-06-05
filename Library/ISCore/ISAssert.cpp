#include "ISAssert.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
void ISASSERT::Assert(bool Boolean, const std::string &FalseMessage, const char *FileName, int Line)
{
    if (!Boolean)
    {
        ShowAssert(FalseMessage, FileName, Line);
    }
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(int Integer, const std::string &FalseMessage, const char *FileName, int Line)
{
    if (!Integer)
    {
        ShowAssert(FalseMessage, FileName, Line);
    }
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(const std::string &String, const std::string &FalseMessage, const char *FileName, int Line)
{
    if (String.empty())
    {
        ShowAssert(FalseMessage, FileName, Line);
    }
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(size_t SizeT, const std::string &FalseMessage, const char *FileName, int Line)
{
    if (!SizeT)
    {
        ShowAssert(FalseMessage, FileName, Line);
    }
}
//-----------------------------------------------------------------------------
void ISASSERT::ShowAssert(const std::string &FalseMessage, const char *FileName, int Line)
{
    //Формируем строку с описание ассерта
    std::string String = ISAlgorithm::StringF("%s\nSource file: %s Line: %d",
        FalseMessage.c_str(), FileName, Line);

    //Логируем и немного ждём
    ISLOGGER_A(String.c_str());
    ISSleep(500);

    //Вызываем исключение
    throw std::runtime_error(String);
}
//-----------------------------------------------------------------------------
