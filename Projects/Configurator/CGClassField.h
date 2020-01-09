#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClassField.h"
//-----------------------------------------------------------------------------
class CGClassField : public QObject
{
	Q_OBJECT

public:
	CGClassField();
	virtual ~CGClassField();

	static bool CheckExistClassField(PMetaClassField *MetaField);
	static void InsertClassField(const ISUuid &ClassUID, PMetaClassField *MetaField);
	static void UpdateClassField(const ISUuid &ClassUID, PMetaClassField *MetaField);
};
//-----------------------------------------------------------------------------
