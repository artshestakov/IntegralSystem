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

	bool Has(const QString &QueryType); //�������� ������� �������
	void Execute(const QString &QueryType, const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer);

private:
	bool CheckField(const QVariantMap &Parameters, const ISVectorString &Fields, ISTcpAnswer &TcpAnswer); //�������� ���� ���������� �� ����������

private:
	void TestQuery(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer); //�����������
	void Sleep(const QVariantMap &Parameters, ISTcpAnswer &TcpAnswer); //��������

private:
	std::map<QString, std::function<void(ISTcpApi &TcpServer, const QVariantMap &, ISTcpAnswer &TcpAnswer)>> Functions;
};
//-----------------------------------------------------------------------------
#endif
