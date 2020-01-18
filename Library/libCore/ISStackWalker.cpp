#include "ISStackWalker.h"
//-----------------------------------------------------------------------------
ISStackWalker::ISStackWalker() : StackWalker(RetrieveLine)
{

}
//-----------------------------------------------------------------------------
void ISStackWalker::OnOutput(LPCSTR szText)
{
	StackString += QString(szText);
}
//-----------------------------------------------------------------------------
QString ISStackWalker::GetCallStack() const
{
	return StackString;
}
//-----------------------------------------------------------------------------
