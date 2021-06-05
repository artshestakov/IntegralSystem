#include "ISStackWalker.h"
//-----------------------------------------------------------------------------
ISStackWalker::ISStackWalker() : StackWalker(RetrieveLine)
{

}
//-----------------------------------------------------------------------------
ISStackWalker::~ISStackWalker()
{

}
//-----------------------------------------------------------------------------
const std::string& ISStackWalker::GetStack() const
{
    return String;
}
//-----------------------------------------------------------------------------
void ISStackWalker::OnOutput(LPCSTR szText)
{
    String += szText;
    StackWalker::OnOutput(szText);
}
//-----------------------------------------------------------------------------
