#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
class ISHTMLHelper : public QObject
{
	Q_OBJECT

public:
	ISHTMLHelper();
	virtual ~ISHTMLHelper();

	static QString GenerateTableFromQuerySelect(const QString &QueryText);
	static QString GenerateTableFromQuerySelect(ISQuery Query);
};
//-----------------------------------------------------------------------------
