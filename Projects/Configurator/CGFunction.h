#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClassFunction.h"
//-----------------------------------------------------------------------------
class CGFunction : public QObject
{
	Q_OBJECT

public:
	CGFunction();
	virtual ~CGFunction();

	static void CreateOrReplaceFunction(PMetaClassFunction *MetaFunction, QString &ErrorString); //Создание функции
};
//-----------------------------------------------------------------------------
