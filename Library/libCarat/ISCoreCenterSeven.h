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
	void SuccessfulAuth(const ISStringMap &StringMap);
	void UserEvent(const ISStringMap &StringMap);

private:
	ISAsteriskSocket *AsteriskSocket;
};
//-----------------------------------------------------------------------------
