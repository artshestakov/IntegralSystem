#include "ISAssert.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
void ISASSERT::Assert(QObject *Object, const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName)
{
	if (!Object)
	{
		ShowAssert(FalseMessage, FunctionName, Line, FileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(bool Boolean, const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName)
{
	if (!Boolean)
	{
		ShowAssert(FalseMessage, FunctionName, Line, FileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(int Integer, const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName)
{
	if (!Integer)
	{
		ShowAssert(FalseMessage, FunctionName, Line, FileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(const QString &String, const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName)
{
	if (!String.length())
	{
		ShowAssert(FalseMessage, FunctionName, Line, FileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(size_t SizeT, const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName)
{
	if (!SizeT)
	{
		ShowAssert(FalseMessage, FunctionName, Line, FileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::ShowAssert(const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName)
{
    ISLOGGER_A(FalseMessage);
	std::stringstream StringStream;
	StringStream << "Assert: " << FalseMessage.toStdString() << std::endl;
	StringStream << "Source file: " << FileName << std::endl;
	StringStream << "Line: " << Line << std::endl;
	StringStream << "Function: " << FunctionName;
    throw std::runtime_error(StringStream.str());
}
//-----------------------------------------------------------------------------
