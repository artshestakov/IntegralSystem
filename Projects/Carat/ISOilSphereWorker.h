#pragma once
#ifndef _ISOILSPHEREWORKER_H_INCLUDED
#define _ISOILSPHEREWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISTcpWorker.h"
//-----------------------------------------------------------------------------
class ISOilSphereWorker : public ISTcpWorker
{
	Q_OBJECT

public:
	Q_INVOKABLE ISOilSphereWorker();
	virtual ~ISOilSphereWorker();

protected:
	bool Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer) override;

private:
	bool PeriodContains(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetStockList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool StatementAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetGasStation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetDebtImplementation(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetDebtCounterparty(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	bool GetUserConsumption(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
};
//-----------------------------------------------------------------------------
#endif
