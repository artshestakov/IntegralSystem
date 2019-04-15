#pragma once
//-----------------------------------------------------------------------------
#include "isprinting_global.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
class ISPRINTING_EXPORT ISHTMLHelper : public QObject
{
	Q_OBJECT

public:
	ISHTMLHelper();
	virtual ~ISHTMLHelper();

	static QString GenerateTableFromQuerySelect(const QString &QueryText);
	static QString GenerateTableFromQuerySelect(ISQuery Query);
};
//-----------------------------------------------------------------------------
