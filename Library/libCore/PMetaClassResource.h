#pragma once
#ifndef _PMETACLASSRESOURCE_H_INCLUDED
#define _PMETACLASSRESOURCE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISUuid.h"
#include "ISTypes.h"
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT PMetaClassResource
{
	void AddField(const QString &FieldName, const QString &Value) //�������� �������� � ��� �������� � ������
	{
		if (FieldName.toLower() != "uid")
		{
			Parameters.insert(FieldName, Value);
		}
	}

	QString TableName;
	ISUuid UID;
	QStringMap Parameters;
};
//-----------------------------------------------------------------------------
#endif
