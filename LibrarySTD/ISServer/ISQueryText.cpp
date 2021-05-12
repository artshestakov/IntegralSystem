#include "ISQueryText.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISQueryText::ISQueryText()
    : PreparedCount(0)
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
std::string ISQueryText::Add(const std::string &SqlText, int ParameterCount)
{
    Map.emplace(SqlText, ParameterCount);
    return SqlText;
}
//-----------------------------------------------------------------------------
void ISQueryText::IsNeedPrepare(const std::string &SqlText, std::string &Hash, int &ParamCount, bool &Prepared)
{
    CRITICAL_SECTION_LOCK(&CS);
    Prepared = false;
    for (int i = 0; i < PreparedCount; ++i)
    {
        const ISSqlPrepare &SqlPrepare = VectorPrepared[i];
        if (SqlPrepare.SqlText == SqlText && SqlPrepare.ThreadID == CURRENT_THREAD_ID())
        {
            Hash = SqlPrepare.Hash;
            ParamCount = Map[SqlText];
            Prepared = true;
            break;
        }
    }
    CRITICAL_SECTION_UNLOCK(&CS);
}
//-----------------------------------------------------------------------------
void ISQueryText::AddPrepared(const std::string &SqlText, const std::string &Hash, int ParamCount)
{
    CRITICAL_SECTION_LOCK(&CS);
    VectorPrepared.emplace_back(ISSqlPrepare{ SqlText, Hash, CURRENT_THREAD_ID(), ParamCount });
    ++PreparedCount;
    CRITICAL_SECTION_UNLOCK(&CS);
}
//-----------------------------------------------------------------------------
