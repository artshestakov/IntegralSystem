#include "ISBlockedIP.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static std::string QS_IP = PREPARE_QUERY("SELECT blip_ip FROM blockedip");
//-----------------------------------------------------------------------------
ISBlockedIP::ISBlockedIP()
    : ErrorString(STRING_NO_ERROR),
    VectorSize(0)
{
    CRITICAL_SECTION_INIT(&CS);
}
//-----------------------------------------------------------------------------
ISBlockedIP::~ISBlockedIP()
{
    CRITICAL_SECTION_DESTROY(&CS);
}
//-----------------------------------------------------------------------------
ISBlockedIP& ISBlockedIP::Instance()
{
    static ISBlockedIP BlockedIP;
    return BlockedIP;
}
//-----------------------------------------------------------------------------
const std::string& ISBlockedIP::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISBlockedIP::Init()
{
    ISQuery qSelect(QS_IP);
    if (!qSelect.Execute())
    {
        ErrorString = qSelect.GetErrorString();
        return false;
    }
    VectorSize = (size_t)qSelect.GetResultRowCount();

    while (qSelect.Next())
    {
        Vector.emplace_back(qSelect.ReadColumn(0));
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISBlockedIP::IsLock(const std::string &IPAddress)
{
    //Проверим наличие такого адреса
    CRITICAL_SECTION_LOCK(&CS);
    bool Result = ISAlgorithm::VectorContains(Vector, IPAddress);
    CRITICAL_SECTION_UNLOCK(&CS);
    
    if (!Result) //Такого адреса нет, проверим маску
    {

    }
    //Такой адрес есть
    return Result;
}
//-----------------------------------------------------------------------------
