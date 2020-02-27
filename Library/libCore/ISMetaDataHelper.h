#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISMetaDataHelper
{
public:
	static QString GenerateSqlQueryFromForeign(PMetaClassForeign *MetaForeign, const QString &SqlFilter = QString(), const QVariant &ObjectID = QVariant()); //��������� SQL-������� ��� �������� �����
	static QString GetConfigurationName(); //�������� ������������ ������������ ��
};
//-----------------------------------------------------------------------------
