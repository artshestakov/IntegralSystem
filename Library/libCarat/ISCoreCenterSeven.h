#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISAsteriskSocket.h"
//-----------------------------------------------------------------------------
class LIBCARAT_EXPORT ISCoreCenterSeven : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreCenterSeven(int &argc, char **argv);
	virtual ~ISCoreCenterSeven();

	bool Invoke() override;

protected:
	void SuccessfulAuth(const QStringMap &StringMap);
	void UserEvent(const QStringMap &StringMap);

private:
	ISAsteriskSocket *AsteriskSocket;
};
//-----------------------------------------------------------------------------
