#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISAsteriskSocket.h"
//-----------------------------------------------------------------------------
class ISCARAT_EXPORT ISCoreHighway : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreHighway(int &argc, char **argv);
	virtual ~ISCoreHighway();

	bool Invoke() override;

protected:
	void SuccessfulAuth(const ISStringMap &StringMap);
	void UserEvent(const ISStringMap &StringMap);

private:
	ISAsteriskSocket *AsteriskSocket;
};
//-----------------------------------------------------------------------------
