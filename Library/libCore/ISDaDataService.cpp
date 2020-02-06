#include "ISDaDataService.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
ISDaDataService::ISDaDataService(QObject *parent) : QObject(parent)
{
	RestWorker = new ISRestWorker(this);
	RestWorker->SetToken(TOKEN_DA_DATA_TOKEN);
	RestWorker->initRequester(QString(), 0, nullptr);
	connect(RestWorker, &ISRestWorker::Success, this, &ISDaDataService::SuccessWorker);
	connect(RestWorker, &ISRestWorker::Error, this, &ISDaDataService::ErrorWorker);
}
//-----------------------------------------------------------------------------
ISDaDataService::~ISDaDataService()
{

}
//-----------------------------------------------------------------------------
const QVariantMap& ISDaDataService::GetData() const
{
	return DataMap;
}
//-----------------------------------------------------------------------------
void ISDaDataService::SearchFromINN(const QString &INN)
{
	QVariantMap VariantMap;
	VariantMap.insert("Content-Type", "appliaction/json");
	VariantMap.insert("Accept", "appliaction/json");
	VariantMap.insert("Authorization", QString("Token %1").arg(RestWorker->GetToken()));
	VariantMap.insert("query", INN);
	RestWorker->sendRequest("https://suggestions.dadata.ru/suggestions/api/4_1/rs/suggest/party", ISRestWorker::QueryType::POST, VariantMap);
}
//-----------------------------------------------------------------------------
void ISDaDataService::SuccessWorker(const QJsonObject &JsonObject)
{
	DataMap.clear();

	QVariantMap VariantMap = JsonObject.toVariantMap();
	QVariant Suggestions = VariantMap["suggestions"];
	QVariantList VariantList = Suggestions.toList();
	if (VariantList.count())
	{
		VariantMap = VariantList.at(0).toMap();

		QVariantMap Data = VariantMap.value("data").toMap();
		DataMap.insert("name", Data.value("name").toMap().value("short_with_opf"));
		DataMap.insert("kpp", Data.value("kpp"));
		DataMap.insert("address", Data.value("address").toMap().value("unrestricted_value"));
		DataMap.insert("management", Data.value("management").toMap().value("name"));
		DataMap.insert("okved", Data.value("okved"));
	}

	emit Finished();
}
//-----------------------------------------------------------------------------
void ISDaDataService::ErrorWorker(const QJsonObject &JsonObject)
{
    Q_UNUSED(JsonObject);
}
//-----------------------------------------------------------------------------
