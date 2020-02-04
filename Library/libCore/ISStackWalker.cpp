#include "ISStackWalker.h"
//-----------------------------------------------------------------------------
ISStackWalker::ISStackWalker() : StackWalker(RetrieveLine)
{

}
//-----------------------------------------------------------------------------
void ISStackWalker::OnOutput(LPCSTR Data)
{
	String += Data;
}
//-----------------------------------------------------------------------------
std::string ISStackWalker::GetCallStack() const
{
	return String;
}
//-----------------------------------------------------------------------------
