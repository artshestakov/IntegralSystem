#pragma once
#ifndef _ISBLOCKEDIP_H_INCLUDED
#define _ISBLOCKEDIP_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISBlockedIP
{
public:
    static ISBlockedIP& Instance();

    const std::string& GetErrorString() const;
    bool Init();
    bool IsLock(const std::string &IPAddress);

private:
    ISBlockedIP();
    ~ISBlockedIP();
    ISBlockedIP(const ISBlockedIP&) = delete;
    ISBlockedIP(ISBlockedIP&&) = delete;
    ISBlockedIP& operator=(const ISBlockedIP&) = delete;
    ISBlockedIP& operator=(ISBlockedIP&&) = delete;

private:
    std::string ErrorString;
    ISCriticalSection CS;
    std::vector<std::string> Vector;
    size_t VectorSize;
};
//-----------------------------------------------------------------------------
#endif
