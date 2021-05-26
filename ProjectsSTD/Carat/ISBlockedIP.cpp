#include "ISBlockedIP.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static std::string QS_REGEXP = PREPARE_QUERY("SELECT blip_regexp FROM _blockedip");
//-----------------------------------------------------------------------------
ISBlockedIP::ISBlockedIP()
    : ErrorString(STRING_NO_ERROR)
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
    ISQuery qSelect(QS_REGEXP);
    if (!qSelect.Execute())
    {
        ErrorString = qSelect.GetErrorString();
        return false;
    }

    while (qSelect.Next())
    {
        Vector.emplace_back(qSelect.ReadColumn(0));
    }
    return true;
}
//-----------------------------------------------------------------------------
bool ISBlockedIP::IsLock(const std::string &IPAddress)
{
    bool Result = false;
    CRITICAL_SECTION_LOCK(&CS);

    //Переменные для проверки
    std::regex RegExp;
    std::smatch Match;
    for (const std::string &RegExpString : Vector) //Обходим все выражения
    {
        RegExp = RegExpString; //Выбираем текущее выражение
        Result = regex_match(IPAddress, Match, RegExp);
        if (Result) //Если адрес подходит под текущее выражение - выходим
        {
            break;
        }
    }
    CRITICAL_SECTION_UNLOCK(&CS);
    return Result;
}
//-----------------------------------------------------------------------------
void ISBlockedIP::Add(const std::string &RegExp)
{
    CRITICAL_SECTION_LOCK(&CS);
    Vector.emplace_back(RegExp);
    CRITICAL_SECTION_UNLOCK(&CS);
}
//-----------------------------------------------------------------------------
