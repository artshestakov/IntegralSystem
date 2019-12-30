#pragma once
//-----------------------------------------------------------------------------
#include "libcore_global.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISHTMLHelper : public QObject
{
	Q_OBJECT

public:
	ISHTMLHelper();
	virtual ~ISHTMLHelper();

	static QString GenerateTableFromQuerySelect(const QString &QueryText);
	static QString GenerateTableFromQuerySelect(ISQuery Query);
};
//-----------------------------------------------------------------------------
