#include "ISDaDataService.h"
#include "ISConstants.h"
#include "ISLocalization.h"
//-----------------------------------------------------------------------------
ISDaDataService::ISDaDataService(QObject *parent) : QObject(parent)
{
	RestWorker = new ISRestWorker(this);
	RestWorker->SetToken(TOKEN_DA_DATA_TOKEN);
	RestWorker->initRequester(QString(), 0, nullptr);
	connect(RestWorker, &ISRestWorker::Error, this, &ISDaDataService::ErrorWorker);
}
//-----------------------------------------------------------------------------
ISDaDataService::~ISDaDataService()
{

}
//-----------------------------------------------------------------------------
void ISDaDataService::OrganizationFromINN(const QString &INN)
{
	connect(RestWorker, &ISRestWorker::Success, this, &ISDaDataService::SuccessOrganization);
	QVariantMap VariantMap
	{
		{ "Content-Type", "appliaction/json" },
		{ "Accept", "appliaction/json" },
		{ "Authorization", QString("Token ") + TOKEN_DA_DATA_TOKEN },
		{ "query", INN }
	};
	RestWorker->sendRequest("https://suggestions.dadata.ru/suggestions/api/4_1/rs/suggest/party", ISRestWorker::QueryType::POST, VariantMap);
}
//-----------------------------------------------------------------------------
void ISDaDataService::BankFromBIK(const QString &BIK)
{
	connect(RestWorker, &ISRestWorker::Success, this, &ISDaDataService::SuccessBank);
	QVariantMap VariantMap
	{
		{ "Content-Type", "appliaction/json" },
		{ "Accept", "appliaction/json" },
		{ "Authorization", QString("Token ") + TOKEN_DA_DATA_TOKEN },
		{ "query", BIK }
	};
	RestWorker->sendRequest("https://suggestions.dadata.ru/suggestions/api/4_1/rs/findById/bank", ISRestWorker::QueryType::POST, VariantMap);
}
//-----------------------------------------------------------------------------
void ISDaDataService::SuccessOrganization(const QJsonObject &JsonObject)
{
	QVariantMap VariantMap = JsonObject.toVariantMap();
	QVariant Suggestions = VariantMap["suggestions"];
	QVariantList VariantList = Suggestions.toList();
	VariantMap = VariantList[0].toMap();

	ISDaDataOrganization Organization;
	Organization.Value = VariantMap["value"].toString();
	Organization.Address = VariantMap["data"].toMap()["address"].toMap()["value"].toString();
	Organization.Branch.Count = VariantMap["data"].toMap()["branch_count"].toInt();
	Organization.Branch.BranchType = VariantMap["data"].toMap()["branch_type"].toString();
	Organization.Branch.BranchTypeLocalName = LANG("DaData.Organization.BranchType." + Organization.Branch.BranchType);
	Organization.Inn = VariantMap["data"].toMap()["inn"].toString();
	Organization.Kpp = VariantMap["data"].toMap()["kpp"].toString();
	Organization.Ogrn = VariantMap["data"].toMap()["ogrn"].toString();
	Organization.OrgnDate = VariantMap["data"].toMap()["ogrn_date"].toString();
	Organization.HID = VariantMap["data"].toMap()["hid"].toString();
	Organization.Management.Name = VariantMap["data"].toMap()["management"].toMap()["name"].toString();
	Organization.Management.Post = VariantMap["data"].toMap()["management"].toMap()["post"].toString();
	Organization.Name.FullWithOPF = VariantMap["data"].toMap()["name"].toMap()["full_with_opf"].toString();
	Organization.Name.ShortWithOPF = VariantMap["data"].toMap()["name"].toMap()["short_with_opf"].toString();
	Organization.Name.Full = VariantMap["data"].toMap()["name"].toMap()["full"].toString();
	Organization.Name.Short = VariantMap["data"].toMap()["name"].toMap()["short"].toString();
	Organization.Okved.Okved = VariantMap["data"].toMap()["okved"].toString();
	Organization.Okved.Version = VariantMap["data"].toMap()["okved_type"].toInt();
	Organization.OPF.Code = VariantMap["data"].toMap()["opf"].toMap()["code"].toString();
	Organization.OPF.FullName = VariantMap["data"].toMap()["opf"].toMap()["full"].toString();
	Organization.OPF.ShortName = VariantMap["data"].toMap()["opf"].toMap()["short"].toString();
	Organization.OPF.Type = VariantMap["data"].toMap()["opf"].toMap()["type"].toInt();
	Organization.State.ActualityDate = VariantMap["data"].toMap()["state"].toMap()["actuality_date"].toString();
	Organization.State.RegistrationDate = VariantMap["data"].toMap()["state"].toMap()["registration_date"].toString();
	Organization.State.LiquidationDate = VariantMap["data"].toMap()["state"].toMap()["liquidation_date"].toString();
	Organization.State.StatusName = VariantMap["data"].toMap()["state"].toMap()["status"].toString();
	Organization.State.StatusLocalName = LANG("DaData.Organization.State." + Organization.State.StatusName);
	Organization.Type = VariantMap["data"].toMap()["type"].toString();
	Organization.TypeLocalName = LANG("DaData.Organization.Type." + Organization.Type);
	emit OgranizationFounded(Organization);
}
//-----------------------------------------------------------------------------
void ISDaDataService::SuccessBank(const QJsonObject &JsonObject)
{
	QVariantMap VariantMap = JsonObject.toVariantMap();
	QVariant Suggestions = VariantMap["suggestions"];
	QVariantList VariantList = Suggestions.toList();
	VariantMap = VariantList[0].toMap();

	ISDaDataBank Bank;
	Bank.Value = VariantMap["value"].toString();
	Bank.Address = VariantMap["data"].toMap()["address"].toMap()["value"].toString();
	Bank.BIK = VariantMap["data"].toMap()["bic"].toString();
	Bank.Swift = VariantMap["data"].toMap()["swift"].toString();
	Bank.RegistrationNumber = VariantMap["data"].toMap()["registration_number"].toString();
	Bank.CorrespondentAccount = VariantMap["data"].toMap()["correspondent_account"].toString();
	Bank.Name.Payment = VariantMap["data"].toMap()["name"].toMap()["payment"].toString();
	Bank.Name.Short = VariantMap["data"].toMap()["name"].toMap()["short"].toString();
	Bank.PaymentCity = VariantMap["data"].toMap()["payment_city"].toString();
	Bank.OPF = VariantMap["data"].toMap()["opf"].toMap()["type"].toString();
	Bank.OPFLocalName = LANG("DaData.Bank.OPF.Type." + Bank.OPF);
	Bank.State.ActualityDate = VariantMap["data"].toMap()["state"].toMap()["actuality_date"].toString();
	Bank.State.RegistrationDate = VariantMap["data"].toMap()["state"].toMap()["registration_date"].toString();
	Bank.State.LiquidationDate = VariantMap["data"].toMap()["state"].toMap()["liquidation_date"].toString();
	Bank.State.StatusName = VariantMap["data"].toMap()["state"].toMap()["status"].toString();
	Bank.State.StatusLocalName = LANG("DaData.Bank.State." + Bank.State.StatusName);
	emit BankFounded(Bank);
}
//-----------------------------------------------------------------------------
void ISDaDataService::ErrorWorker(const QJsonObject &JsonObject)
{
    Q_UNUSED(JsonObject);
}
//-----------------------------------------------------------------------------
