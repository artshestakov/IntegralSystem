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
	void Finished(const ISOrganizationStruct &OrganizationStruct);

public:
	ISDaDataService(QObject *parent = 0);
	virtual ~ISDaDataService();

	void SearchFromINN(const QString &INN);

protected:
	void SuccessWorker(const QJsonObject &JsonObject);
	void ErrorWorker(const QJsonObject &JsonObject);

private:
	ISRestWorker *RestWorker;
};
//-----------------------------------------------------------------------------
#endif
