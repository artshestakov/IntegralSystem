#include "StdAfx.h"
#include "ISAssert.h"
#include "EXDefines.h"
#include "ISDebug.h"
#include "ISSystem.h"
#include "ISExceptionAssert.h"
//-----------------------------------------------------------------------------
ISASSERT::ISASSERT()
{

}
//-----------------------------------------------------------------------------
ISASSERT::~ISASSERT()
{

}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(QObject *Object, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &File)
{
	if (!Object)
	{
		ShowAssert(FalseMessage, QFuncInfo, Line, File);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(bool Boolean, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &File)
{
	if (!Boolean)
	{
		ShowAssert(FalseMessage, QFuncInfo, Line, File);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(int Integer, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &File)
{
	if (!Integer)
	{
		ShowAssert(FalseMessage, QFuncInfo, Line, File);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(QString String, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &File)
{
	if (!String.length())
	{
		ShowAssert(FalseMessage, QFuncInfo, Line, File);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::ShowAssert(const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &File)
{
	QString Message = QString("Message: %1").arg(FalseMessage);
	QString SourceFile = QString("Source file: %1").arg(QFileInfo(File).fileName());
	QString SourceLine = QString("Source line: %1").arg(Line);
	QString Function = QString("Function: %1").arg(ISSystem::FormatQFuncInfo(QFuncInfo, ISNamespace::FNF_TypeAndFunction));

	QString AssertMessage = QString("%1\r\n%2\r\n%3\r\n%4").arg(Message).arg(SourceFile).arg(SourceLine).arg(Function);
	ISDebug::ShowAssertString(AssertMessage);

	ISDebug::GetInstance().SetAssertMessage(AssertMessage);
    throw runtime_error(AssertMessage.toStdString().c_str());
}
//-----------------------------------------------------------------------------
