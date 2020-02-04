#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class LIBCARAT_EXPORT ISCoreSMS : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreSMS(int &argc, char **argv);
	virtual ~ISCoreSMS();

	bool Invoke() override;

protected:
	void Timeout();
	void UpdateStatus(int ID, const ISUuid &StatusUID);
	void SendDone(int ID, int MessageID);
	void SendFailed(int ID, int MessageID, const QString &ErrorString);

	QString GetCharset(const ISUuid &CharsetUID) const;

private:
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
