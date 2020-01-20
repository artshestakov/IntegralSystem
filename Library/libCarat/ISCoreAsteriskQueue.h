#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISAsteriskSocket.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISCoreAsteriskQueue : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreAsteriskQueue(int &argc, char **argv);
	virtual ~ISCoreAsteriskQueue();

	bool Invoke() override;

protected:
	void SuccessfulAuth();
	void Timeout();
	void QueueEvent(const ISUuid &EventTypeUID, int UserID, const QString &Parameters); //Событие очереди
	void Originate(int UserID, const QVariantMap &Parameters); //Исходящий вызов
	void ClearQueue(); //Очистка очереди

private:
	ISAsteriskSocket *AsteriskSocket;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
