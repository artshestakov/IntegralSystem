#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISRegisterMetaType : public QObject
{
	Q_OBJECT

public:
	ISRegisterMetaType();
	virtual ~ISRegisterMetaType();

	static void RegisterMetaType(); //Регистрация мета-типов
};
//-----------------------------------------------------------------------------
