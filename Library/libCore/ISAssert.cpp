#include "ISAssert.h"
#include "ISCrashDumper.h"
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
void ISASSERT::Assert(QString String, const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName)
{
	if (!String.length())
	{
		ShowAssert(FalseMessage, FunctionName, Line, FileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::ShowAssert(const QString &FalseMessage, const char *FunctionName, int Line, const char *FileName)
{
	std::stringstream StringStream;
	StringStream << "Assert: " << FalseMessage.toStdString() << std::endl;
	StringStream << "Source file: " << FileName << std::endl;
	StringStream << "Line: " << Line << std::endl;
	StringStream << "Function: " << FunctionName;
	std::string String = StringStream.str();

	ISCrashDumper::SetAssertMessage(String.c_str());
    throw std::runtime_error(String);
}
//-----------------------------------------------------------------------------
