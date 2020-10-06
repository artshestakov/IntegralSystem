#include "ISCountingTime.h"
//-----------------------------------------------------------------------------
ISCountingTime::ISCountingTime()
    : StartClock(std::chrono::steady_clock::now())
{

}
//-----------------------------------------------------------------------------
ISCountingTime::~ISCountingTime()
{

}
//-----------------------------------------------------------------------------
unsigned long long ISCountingTime::Elapsed()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - StartClock).count();
}
//-----------------------------------------------------------------------------
