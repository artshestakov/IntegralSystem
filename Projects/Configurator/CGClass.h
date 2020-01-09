#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class CGClass : public QObject
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
