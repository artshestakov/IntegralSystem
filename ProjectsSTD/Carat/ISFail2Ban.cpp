#include "ISFail2Ban.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISFail2Ban::ISFail2Ban()
{

}
//-----------------------------------------------------------------------------
ISFail2Ban::~ISFail2Ban()
{

}
//-----------------------------------------------------------------------------
ISFail2Ban& ISFail2Ban::Instance()
{
    static ISFail2Ban Fail2Ban;
    return Fail2Ban;
}
//-----------------------------------------------------------------------------
bool ISFail2Ban::Exist(const std::string &IPAddress)
{
    size_t Index = 0;
    return Exist(IPAddress, Index);
}
//-----------------------------------------------------------------------------
bool ISFail2Ban::Exist(const std::string &IPAddress, size_t &Index)
{
    bool Result = false;
    for (size_t i = 0, c = Vector.size(); i < c; ++i)
    {
        if (Vector[i].IPAddress == IPAddress)
        {
            Result = true;
            Index = i;
            break;
        }
    }
    return Result;
}
//-----------------------------------------------------------------------------
void ISFail2Ban::FailAuth(const std::string &IPAddress, size_t &AttemptLeft)
{
    size_t Index = 0;
    if (Exist(IPAddress, Index))
    {
        AttemptLeft = --Vector[Index].AttemptLeft;
    }
    else
    {
        ISLockedIP LockedIP(IPAddress);
        AttemptLeft = --LockedIP.AttemptLeft;
        Vector.emplace_back(LockedIP);
    }
}
//-----------------------------------------------------------------------------
bool ISFail2Ban::IsEmptyAttempts(const std::string &IPAddress)
{
    uint64_t DateTimeUnlock = 0;
    return IsEmptyAttempts(IPAddress, DateTimeUnlock);
}
//-----------------------------------------------------------------------------
bool ISFail2Ban::IsEmptyAttempts(const std::string &IPAddress, uint64_t &DateTimeUnlock)
{
    size_t Index = 0;
    if (Exist(IPAddress, Index))
    {
        const ISLockedIP &LockedIP = Vector[Index];
        DateTimeUnlock = LockedIP.DateTimeUnlock;
        return LockedIP.IsLock();
    }
    return false;
}
//-----------------------------------------------------------------------------
void ISFail2Ban::Remove(const std::string &IPAddress)
{
    size_t Index = 0;
    if (Exist(IPAddress, Index))
    {
        Vector.erase(Vector.begin() + Index);
    }
}
//-----------------------------------------------------------------------------
