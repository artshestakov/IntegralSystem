#pragma once
//-----------------------------------------------------------------------------
#include "isconfigurator_global.h"
#include "PMetaClassFunction.h"
//-----------------------------------------------------------------------------
class ISCONFIGURATOR_EXPORT CGFunction : public QObject
{
	Q_OBJECT

public:
	CGFunction();
	virtual ~CGFunction();

	static void CreateOrReplaceFunction(PMetaClassFunction *MetaFunction, QString &ErrorString); //Создание функции
};
//-----------------------------------------------------------------------------
