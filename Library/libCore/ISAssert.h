#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
//-----------------------------------------------------------------------------
#define IS_ASSERT(CONDITION, FALSE_MESSAGE) ISASSERT::Assert(CONDITION, FALSE_MESSAGE, Q_FUNC_INFO, SOURCE_LINE, SOURCE_FILE);
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISASSERT
{
public:
	ISASSERT();
	virtual ~ISASSERT();

	static void Assert(QObject *Object, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &SourceFileName);
	static void Assert(bool Boolean, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &SourceFileName);
	static void Assert(int Integer, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &SourceFileName);
	static void Assert(QString String, const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &SourceFileName);

protected:
	static void ShowAssert(const QString &FalseMessage, const QString &QFuncInfo, int Line, const QString &File);
};
//-----------------------------------------------------------------------------
