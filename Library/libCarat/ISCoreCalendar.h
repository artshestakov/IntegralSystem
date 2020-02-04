#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
//-----------------------------------------------------------------------------
class LIBCARAT_EXPORT ISCoreCalendar : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreCalendar(int &argc, char **argv);
	virtual ~ISCoreCalendar();

	bool Invoke() override;

protected:
	void Timeout();

private:
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
