#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "PMetaClassForeign.h"
//-----------------------------------------------------------------------------
class ISMetaDataHelper
{
public:
	static QString GenerateSqlQueryFromForeign(PMetaClassForeign *MetaForeign, const QString &SqlFilter = QString(), const QVariant &ObjectID = QVariant()); //��������� SQL-������� ��� �������� �����
};
//-----------------------------------------------------------------------------
