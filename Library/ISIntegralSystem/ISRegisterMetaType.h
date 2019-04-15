#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISRegisterMetaType : public QObject
{
	Q_OBJECT

public:
	ISRegisterMetaType();
	virtual ~ISRegisterMetaType();

	static void RegisterMetaType(); //Регистрация мета-типов
};
//-----------------------------------------------------------------------------
