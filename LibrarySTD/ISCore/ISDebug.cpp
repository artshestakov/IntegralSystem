#include "ISDebug.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
void ShowDebugString(const char *Severity, const std::string &String)
{
    //Получаем текущую дату
    ISDateTime DT = ISAlgorithm::GetCurrentDateTime();

    //Формируем сообщение
    char Buffer[LOG_BUFFER_SIZE] = { 0 };
    (void)snprintf(Buffer, LOG_BUFFER_SIZE, "%02d.%02d.%02d %02d:%02d:%02d:%03d [%s]\t%s",
        DT.Date.Day, DT.Date.Month, DT.Date.Year % 100, DT.Time.Hour, DT.Time.Minute, DT.Time.Second, DT.Time.Milliseconds,
        Severity, String.c_str());
    std::cout << Buffer << std::endl;
}
//-----------------------------------------------------------------------------
void ShowDebugString(const std::string &String)
{
    std::cout << String << std::endl;
}
//-----------------------------------------------------------------------------
