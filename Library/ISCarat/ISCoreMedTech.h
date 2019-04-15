#pragma once
//-----------------------------------------------------------------------------
#include "iscarat_global.h"
#include "ISCaratCoreApplication.h"
#include "ISAsteriskSocket.h"
//-----------------------------------------------------------------------------
class ISCARAT_EXPORT ISCoreMedTech : public ISCaratCoreApplication
{
	Q_OBJECT

public:
	ISCoreMedTech(int &argc, char **argv);
	virtual ~ISCoreMedTech();

	bool Invoke() override;

protected:
	void SuccessfulAuth(const QStringMap &StringMap);
	void UserEvent(const QStringMap &StringMap); //Обработка качества обслуживания

private:
	ISAsteriskSocket *AsteriskSocket;	
};
//-----------------------------------------------------------------------------
