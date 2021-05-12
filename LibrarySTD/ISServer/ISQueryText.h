#pragma once
#ifndef _ISQUERYTEXT_H_INCLUDED
#define _ISQUERYTEXT_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
#include "ISStructs.h"
//-----------------------------------------------------------------------------
class ISQueryText
{
public:
    static ISQueryText& Instance();

    std::string Add(const std::string &SqlText, int ParameterCount = 0);
    void IsNeedPrepare(const std::string &SqlText, std::string &Hash, int &ParamCount, bool &Prepared);
    void AddPrepared(const std::string &SqlText, const std::string &Hash, int ParamCount);

private:
    ISQueryText();
    ~ISQueryText();
    ISQueryText(const ISQueryText&) = delete;
    ISQueryText(ISQueryText&&) = delete;
    ISQueryText& operator=(const ISQueryText&) = delete;
    ISQueryText& operator=(ISQueryText&&) = delete;

private:
    std::unordered_map<std::string, int> Map; // арта запросов с количеством параметров
    std::vector<ISSqlPrepare> VectorPrepared;
    int PreparedCount;
    ISCriticalSection CS;
};
//-----------------------------------------------------------------------------
#define PREPARE_QUERY(SQL_TEXT) ISQueryText::Instance().Add(SQL_TEXT)
#define PREPARE_QUERYN(SQL_TEXT, PARAMETER_COUNT) ISQueryText::Instance().Add(SQL_TEXT, PARAMETER_COUNT)
//-----------------------------------------------------------------------------
#endif
