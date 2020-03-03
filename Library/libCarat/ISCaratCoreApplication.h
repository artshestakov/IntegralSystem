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

	QString GetErrorString() const;
	virtual bool Invoke() = 0;

protected:
	void Started(); //Уведомление о запуске

private:
	QString ErrorString;
};
//-----------------------------------------------------------------------------
