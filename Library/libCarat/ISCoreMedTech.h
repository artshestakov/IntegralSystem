#pragma once
//-----------------------------------------------------------------------------
#include "ISCaratCoreApplication.h"
#include "ISAsteriskSocket.h"
//-----------------------------------------------------------------------------
class ISCoreMedTech : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreMedTech(int &argc, char **argv);
	virtual ~ISCoreMedTech();

	bool Invoke() override;

protected:
	void SuccessfulAuth(const QStringMap &StringMap);
	void UserEvent(const QStringMap &StringMap); //��������� �������� ������������

private:
	ISAsteriskSocket *AsteriskSocket;	
};
//-----------------------------------------------------------------------------
