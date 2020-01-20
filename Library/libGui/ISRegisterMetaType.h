#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISRegisterMetaType : public QObject
{
	Q_OBJECT

public:
	ISRegisterMetaType();
	virtual ~ISRegisterMetaType();

	static void RegisterMetaType(); //Регистрация мета-типов
};
//-----------------------------------------------------------------------------
