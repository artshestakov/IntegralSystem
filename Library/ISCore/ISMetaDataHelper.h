#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISMetaDataHelper
{
public:
	static QString GenerateSqlQueryFromForeign(PMetaForeign *MetaForeign, const QString &SqlFilter = QString(), const QVariant &ObjectID = QVariant()); //��������� SQL-������� ��� �������� �����
	static QString GenerateSqlQueryFromTitleName(PMetaForeign *MetaForeign, const QString &Alias, const QString &FieldName); //��������� SQL-������� ��� ��������� ����� ��������� ������
};
//-----------------------------------------------------------------------------
