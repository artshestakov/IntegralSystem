#pragma once
#ifndef _ISDADATASERVICE_H_INCLUDED
#define _ISDADATASERVICE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libGui_global.h"
#include "ISStructs.h"
#include "ISRestWorker.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDaDataService : public QObject
{
	Q_OBJECT

signals:
	void OgranizationFounded(const ISDaDataOrganization &);
	void BankFounded(const ISDaDataBank &);

public:
	ISDaDataService(QObject *parent = 0);
	virtual ~ISDaDataService();

	void OrganizationFromINN(const QString &INN); //Поиск организации по ИНН
	void BankFromBIK(const QString &BIK); //Поиск банка по БИК

private:
	void SuccessOrganization(const QJsonObject &JsonObject);
	void SuccessBank(const QJsonObject &JsonObject);

	void ErrorWorker(const QJsonObject &JsonObject);

private:
	ISRestWorker *RestWorker;
};
//-----------------------------------------------------------------------------
#endif
