#pragma once
//-----------------------------------------------------------------------------
#include "isnetwork_global.h"
#include "ISRestWorker.h"
//-----------------------------------------------------------------------------
class ISNETWORK_EXPORT ISDaDataService : public QObject
{
	Q_OBJECT

signals:
	void Finished();

public:
	ISDaDataService(QObject *parent = 0);
	virtual ~ISDaDataService();

	const QVariantMap& GetData() const;
	void SearchFromINN(const QString &INN);

protected:
	void SuccessWorker(const QJsonObject &JsonObject);
	void ErrorWorker(const QJsonObject &JsonObject);

private:
	ISRestWorker *RestWorker;
	QVariantMap DataMap;
};
//-----------------------------------------------------------------------------
