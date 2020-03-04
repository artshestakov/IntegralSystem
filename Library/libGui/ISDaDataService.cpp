#include "ISDaDataService.h"
#include "ISConstants.h"
#include "ISLocalization.h"
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
	QVariantMap VariantMap = JsonObject.toVariantMap();
	QVariant Suggestions = VariantMap["suggestions"];
	QVariantList VariantList = Suggestions.toList();
	VariantMap = VariantList[0].toMap();

	ISOrganizationStruct OrganizationStruct;
	OrganizationStruct.Value = VariantMap["value"].toString();
	OrganizationStruct.Address = VariantMap["data"].toMap()["address"].toMap()["value"].toString();
	OrganizationStruct.Branch.Count = VariantMap["data"].toMap()["branch_count"].toInt();
	OrganizationStruct.Branch.BranchType = VariantMap["data"].toMap()["branch_type"].toString();
	OrganizationStruct.Branch.BranchTypeLocalName = LANG("DaData.BranchType." + OrganizationStruct.Branch.BranchType);
	OrganizationStruct.Inn = VariantMap["data"].toMap()["inn"].toString();
	OrganizationStruct.Kpp = VariantMap["data"].toMap()["kpp"].toString();
	OrganizationStruct.Ogrn = VariantMap["data"].toMap()["ogrn"].toString();
	OrganizationStruct.OrgnDate = VariantMap["data"].toMap()["ogrn_date"].toString();
	OrganizationStruct.HID = VariantMap["data"].toMap()["hid"].toString();
	OrganizationStruct.Management.Name = VariantMap["data"].toMap()["management"].toMap()["name"].toString();
	OrganizationStruct.Management.Post = VariantMap["data"].toMap()["management"].toMap()["post"].toString();
	OrganizationStruct.Name.FullWithOPF = VariantMap["data"].toMap()["name"].toMap()["full_with_opf"].toString();
	OrganizationStruct.Name.ShortWithOPF = VariantMap["data"].toMap()["name"].toMap()["short_with_opf"].toString();
	OrganizationStruct.Name.Full = VariantMap["data"].toMap()["name"].toMap()["full"].toString();
	OrganizationStruct.Name.Short = VariantMap["data"].toMap()["name"].toMap()["short"].toString();
	OrganizationStruct.Okved.Okved = VariantMap["data"].toMap()["okved"].toString();
	OrganizationStruct.Okved.Version = VariantMap["data"].toMap()["okved_type"].toInt();
	OrganizationStruct.OPF.Code = VariantMap["data"].toMap()["opf"].toMap()["code"].toString();
	OrganizationStruct.OPF.FullName = VariantMap["data"].toMap()["opf"].toMap()["full"].toString();
	OrganizationStruct.OPF.ShortName = VariantMap["data"].toMap()["opf"].toMap()["short"].toString();
	OrganizationStruct.OPF.Type = VariantMap["data"].toMap()["opf"].toMap()["type"].toInt();
	OrganizationStruct.State.ActualityDate = VariantMap["data"].toMap()["state"].toMap()["actuality_date"].toString();
	OrganizationStruct.State.RegistrationDate = VariantMap["data"].toMap()["state"].toMap()["registration_date"].toString();
	OrganizationStruct.State.LiquidationDate = VariantMap["data"].toMap()["state"].toMap()["liquidation_date"].toString();
	OrganizationStruct.State.StatusName = VariantMap["data"].toMap()["state"].toMap()["status"].toString();
	OrganizationStruct.State.StatusLocalName = LANG("DaData.State.Status." + OrganizationStruct.State.StatusName);
	OrganizationStruct.Type = VariantMap["data"].toMap()["type"].toString();
	OrganizationStruct.TypeLocalName = LANG("DaData.Type." + OrganizationStruct.Type);
	emit Finished(OrganizationStruct);
}
//-----------------------------------------------------------------------------
void ISDaDataService::ErrorWorker(const QJsonObject &JsonObject)
{
    Q_UNUSED(JsonObject);
}
//-----------------------------------------------------------------------------
