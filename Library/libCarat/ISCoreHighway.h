#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISAsteriskSocket.h"
//-----------------------------------------------------------------------------
class LIBCARAT_EXPORT ISCoreHighway : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreHighway(int &argc, char **argv);
	virtual ~ISCoreHighway();

	bool Invoke() override;

protected:
	void SuccessfulAuth(const QStringMap &StringMap);
	void UserEvent(const QStringMap &StringMap);

private:
	ISAsteriskSocket *AsteriskSocket;
};
//-----------------------------------------------------------------------------
