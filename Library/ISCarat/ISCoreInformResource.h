#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISAsteriskSocket.h"
//-----------------------------------------------------------------------------
class ISCARAT_EXPORT ISCoreInformResource : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreInformResource(int &argc, char **argv);
	virtual ~ISCoreInformResource();

	bool Invoke() override;

protected:
	void SuccessfulAuth(const ISStringMap &StringMap);
	void UserEvent(const ISStringMap &StringMap); //Поиск ответственного

private:
	ISAsteriskSocket *AsteriskSocket;
};
//-----------------------------------------------------------------------------
