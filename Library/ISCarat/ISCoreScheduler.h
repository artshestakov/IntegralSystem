#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
//-----------------------------------------------------------------------------
class ISCARAT_EXPORT ISCoreScheduler : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreScheduler(int &argc, char **argv);
	virtual ~ISCoreScheduler();

	bool Invoke() override;

protected:
	void Timeout();

protected:
	void ClearFiles(); //������� �� ����������� ������

private:
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
