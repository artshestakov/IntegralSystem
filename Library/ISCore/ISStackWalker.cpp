#include "StdAfx.h"
#include "ISStackWalker.h"
//-----------------------------------------------------------------------------
ISStackWalker::ISStackWalker() : StackWalker(RetrieveLine)
{

}
//-----------------------------------------------------------------------------
void ISStackWalker::OnOutput(LPCSTR szText)
{
	QString s(szText);
	StackString += QString(s);
}
//-----------------------------------------------------------------------------
QString ISStackWalker::GetCallStack() const
{
	return StackString;
}
//-----------------------------------------------------------------------------
