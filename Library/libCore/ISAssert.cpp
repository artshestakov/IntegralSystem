#include "ISAssert.h"
#include "ISDefines.h"
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
void ISASSERT::Assert(QObject *Object, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &SourceFileName)
{
	if (!Object)
	{
		ShowAssert(FalseMessage, QFuncInfo, Line, SourceFileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(bool Boolean, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &SourceFileName)
{
	if (!Boolean)
	{
		ShowAssert(FalseMessage, QFuncInfo, Line, SourceFileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(int Integer, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &SourceFileName)
{
	if (!Integer)
	{
		ShowAssert(FalseMessage, QFuncInfo, Line, SourceFileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::Assert(QString String, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &SourceFileName)
{
	if (!String.length())
	{
		ShowAssert(FalseMessage, QFuncInfo, Line, SourceFileName);
	}
}
//-----------------------------------------------------------------------------
void ISASSERT::ShowAssert(const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &SourceFileName)
{
	QString Message = QString("Message: %1").arg(FalseMessage);
	QString SourceFile = QString("Source file: %1").arg(QFileInfo(SourceFileName).fileName());
	QString SourceLine = QString("Source line: %1").arg(Line);
	QString Function = QString("Function: %1").arg(ISSystem::FormatQFuncInfo(QFuncInfo, ISNamespace::FNF_TypeAndFunction));

	QString AssertMessage = QString("%1\r\n%2\r\n%3\r\n%4").arg(Message).arg(SourceFile).arg(SourceLine).arg(Function);
	ISDebug::ShowAssertString(AssertMessage);

	ISDebug::GetInstance().SetAssertMessage(AssertMessage);
    throw std::runtime_error(AssertMessage.toStdString().c_str());
}
//-----------------------------------------------------------------------------
