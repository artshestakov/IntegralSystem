#pragma once
#ifndef _ISTCPAPI_H_INCLUDED
#define _ISTCPAPI_H_INCLUDED
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "ISTcpAnswer.h"
#include "ISTypedefs.h"
//-----------------------------------------------------------------------------
class ISTcpApi
{
public:
	ISTcpApi();
	~ISTcpApi();

	bool Has(const QString &QueryType); //Проверка наличия функции
	void Execute(const QString &QueryType, const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer);

private:
	bool CheckField(const QVariantMap &Parameters, const ISVectorString &Fields, ISTcpAnswer &TcpAnswer); //Проверка поля параметров на валидность

private:
	void Auth(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer); //Авторизация

private:
	std::map<QString, std::function<void(ISTcpApi &TcpServer, const QVariantMap &, ISTcpAnswer &TcpAnswer)>> Functions;
};
//-----------------------------------------------------------------------------
#endif
