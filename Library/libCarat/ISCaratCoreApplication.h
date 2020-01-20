#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISCaratCoreApplication : public QCoreApplication
{
	Q_OBJECT

public:
	ISCaratCoreApplication(int &argc, char **argv);
	virtual ~ISCaratCoreApplication();

	virtual bool Invoke() = 0;

	int Exec() const;
	void Started(); //Уведомление о запуске
};
//-----------------------------------------------------------------------------
