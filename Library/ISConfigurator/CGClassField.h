#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "PMetaClassField.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGClassField : public QObject
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
