#include "ISQueryText.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISQueryText::ISQueryText()
{
    CRITICAL_SECTION_INIT(&CS);
}
//-----------------------------------------------------------------------------
ISQueryText::~ISQueryText()
{
    CRITICAL_SECTION_DESTROY(&CS);
}
//-----------------------------------------------------------------------------
ISQueryText& ISQueryText::Instance()
{
    static ISQueryText QueryText;
    return QueryText;
}
//-----------------------------------------------------------------------------
std::string ISQueryText::Add(const std::string &SqlText, size_t ParameterCount)
{
    if (Map.find(SqlText) == Map.end())
    {
        Map.emplace(SqlText, new ISSqlPrepare(ISAlgorithm::MD5(SqlText), ParameterCount));
    }
    return SqlText;
}
//-----------------------------------------------------------------------------
bool ISQueryText::IsNeedPrepare(const std::string &SqlText, std::string &Hash, int &ParamCount, bool &Prepared)
{
    bool Result = false;
    CRITICAL_SECTION_LOCK(&CS);
    auto It = Map.find(SqlText);
    Result = It != Map.end();
    if (Result)
    {
        Hash = It->second->Hash;
        ParamCount = It->second->ParameterCount;
        Prepared = It->second->Prepared;
    }
    CRITICAL_SECTION_UNLOCK(&CS);
    return Result;
}
//-----------------------------------------------------------------------------
void ISQueryText::SetPrepared(const std::string &SqlText)
{
    CRITICAL_SECTION_LOCK(&CS);
    Map[SqlText]->Prepared = true;
    CRITICAL_SECTION_UNLOCK(&CS);
}
//-----------------------------------------------------------------------------
