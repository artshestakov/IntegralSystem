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
    bool IsNeedPrepare(const std::string &SqlText, std::string &Hash, int &ParamCount, bool &Prepared);
    void SetPrepared(const std::string &SqlText, bool Prepared);

private:
    ISQueryText();
    ~ISQueryText();
    ISQueryText(const ISQueryText&) = delete;
    ISQueryText(ISQueryText&&) = delete;
    ISQueryText& operator=(const ISQueryText&) = delete;
    ISQueryText& operator=(ISQueryText&&) = delete;

private:
    std::unordered_map<std::string, ISSqlPrepare *> Map;
    ISCriticalSection CS;
};
//-----------------------------------------------------------------------------
#define PREPARE_QUERY(SQL_TEXT) ISQueryText::Instance().Add(SQL_TEXT)
#define PREPARE_QUERYN(SQL_TEXT, PARAMETER_COUNT) ISQueryText::Instance().Add(SQL_TEXT, PARAMETER_COUNT)
//-----------------------------------------------------------------------------
#endif
