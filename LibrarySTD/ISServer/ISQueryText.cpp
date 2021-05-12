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

    //Очищаем карту запросов
    std::vector<ISSqlPrepare *> Vector = ISAlgorithm::ConvertUnorderedMapToValues(Map);
    while (!Vector.empty())
    {
        delete ISAlgorithm::VectorTakeBack(Vector);
    }
    Map.clear();
}
//-----------------------------------------------------------------------------
ISQueryText& ISQueryText::Instance()
{
    static ISQueryText QueryText;
    return QueryText;
}
//-----------------------------------------------------------------------------
std::string ISQueryText::Add(const std::string &SqlText, int ParameterCount)
{
    if (Map.find(SqlText) == Map.end())
    {
        Map.emplace(SqlText, new ISSqlPrepare(ISAlgorithm::StringToMD5(SqlText), ParameterCount));
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
void ISQueryText::SetPrepared(const std::string &SqlText, bool Prepared)
{
    CRITICAL_SECTION_LOCK(&CS);
    auto It = Map.find(SqlText);
    if (It != Map.end())
    {
        Map[SqlText]->Prepared = Prepared;
    }
    CRITICAL_SECTION_UNLOCK(&CS);
}
//-----------------------------------------------------------------------------
