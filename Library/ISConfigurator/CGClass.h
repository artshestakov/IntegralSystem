#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGClass : public QObject
{
	Q_OBJECT

public:
	CGClass();
	virtual ~CGClass();

	static bool CheckExistClass(PMetaClassTable *MetaTable);
	static void InsertClass(PMetaClassTable *MetaTable);
	static void UpdateClass(PMetaClassTable *MetaTable);
};
//-----------------------------------------------------------------------------
