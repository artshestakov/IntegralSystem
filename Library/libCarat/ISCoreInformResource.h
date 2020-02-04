#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISAsteriskSocket.h"
//-----------------------------------------------------------------------------
class LIBCARAT_EXPORT ISCoreInformResource : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreInformResource(int &argc, char **argv);
	virtual ~ISCoreInformResource();

	bool Invoke() override;

protected:
	void SuccessfulAuth(const QStringMap &StringMap);
	void UserEvent(const QStringMap &StringMap); //Поиск ответственного

private:
	ISAsteriskSocket *AsteriskSocket;
};
//-----------------------------------------------------------------------------
