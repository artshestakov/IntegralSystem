#pragma once
//-----------------------------------------------------------------------------
#include "libCarat_global.h"
//-----------------------------------------------------------------------------
class LIBCARAT_EXPORT ISCaratCoreApplication : public QCoreApplication
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
