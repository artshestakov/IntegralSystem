#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISIntegralSystem : public QApplication
{
	Q_OBJECT

public:
	ISIntegralSystem(int &argc, char **argv);
	virtual ~ISIntegralSystem();

	bool CheckAdminRole(); //Проверка наличия прав администратора
	bool notify(QObject *Object, QEvent *e);
};
//-----------------------------------------------------------------------------
