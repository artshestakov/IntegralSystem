#include "ISAssert.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
void ISASSERT::Assert(bool Boolean, const std::string &FalseMessage, const std::string &FunctionName, int Line, const char *FileName)
{
	if (!Boolean)
	{
		ShowAssert(FalseMessage, FunctionName, Line, FileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(int Integer, const std::string &FalseMessage, const std::string &FunctionName, int Line, const char *FileName)
{
	if (!Integer)
	{
		ShowAssert(FalseMessage, FunctionName, Line, FileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(const std::string &String, const std::string &FalseMessage, const std::string &FunctionName, int Line, const char *FileName)
{
	if (String.empty())
	{
		ShowAssert(FalseMessage, FunctionName, Line, FileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(size_t SizeT, const std::string &FalseMessage, const std::string &FunctionName, int Line, const char *FileName)
{
	if (!SizeT)
	{
		ShowAssert(FalseMessage, FunctionName, Line, FileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::ShowAssert(const std::string &FalseMessage, const std::string &FunctionName, int Line, const char *FileName)
{
    ISLOGGER_A(FalseMessage.c_str());
	std::stringstream StringStream;
	StringStream << "Assert: " << FalseMessage << std::endl;
	StringStream << "Source file: " << FileName << std::endl;
	StringStream << "Line: " << Line << std::endl;
	StringStream << "Function: " << FunctionName;
    throw std::runtime_error(StringStream.str());
}
//-----------------------------------------------------------------------------
