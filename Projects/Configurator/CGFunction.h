#pragma once
//-----------------------------------------------------------------------------
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class CGFunction
{
public:
	static bool CheckDependencies(PMetaClassFunction *MetaFunction);
	static bool CheckExist(PMetaClassFunction *MetaFunction, QString &ErrorString);
	static void CreateOrReplaceFunction(PMetaClassFunction *MetaFunction, QString &ErrorString); //Создание функции
	static void Delete(PMetaClassFunction *MetaFunction);
};
//-----------------------------------------------------------------------------
