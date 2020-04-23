#pragma once
#ifndef _ISDADATASERVICE_H_INCLUDED
#define _ISDADATASERVICE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISStructs.h"
#include "ISRestWorker.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDaDataService : public QObject
{
	Q_OBJECT

signals:
	void FoundedOgranization(const ISDaDataOrganization &);
	void FoundedBank(const ISDaDataBank &);
	void FoundedLocation(const ISDaDataLocation &);
	void Error(const QString &);

public:
	ISDaDataService(QObject *parent = 0);
	virtual ~ISDaDataService();

	void OrganizationFromINN(const QString &INN); //����� ����������� �� ���
	void BankFromBIK(const QString &BIK); //����� ����� �� ���
	void LocationFromIPAddress(const QString &IPAddress); //����� ������� �� IP-������

private:
	void SuccessOrganization(const QJsonObject &JsonObject);
	void SuccessBank(const QJsonObject &JsonObject);
	void SuccessLocation(const QJsonObject &JsonObject);

	void ErrorWorker(const QJsonObject &JsonObject);

private:
	ISRestWorker *RestWorker;
};
//-----------------------------------------------------------------------------
#endif
