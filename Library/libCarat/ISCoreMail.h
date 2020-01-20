#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISCoreMail : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreMail(int &argc, char **argv);
	virtual ~ISCoreMail();

	bool Invoke() override;

protected:
	void Timeout();
	void SendDone(int MailID);
	void SendFailed(int MailID, const QString &ErrorString);

private:
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
