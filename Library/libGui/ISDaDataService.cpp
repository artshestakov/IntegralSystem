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
void ISDaDataService::LocationFromIPAddress(const QString &IPAddress)
{
	connect(RestWorker, &ISRestWorker::Success, this, &ISDaDataService::SuccessLocation);
	QVariantMap VariantMap
	{
		{ "Accept", "application/json" },
		{ "Authorization", QString("Token ") + TOKEN_DA_DATA_TOKEN }
	};
	RestWorker->sendRequest("https://suggestions.dadata.ru/suggestions/api/4_1/rs/iplocate/address?ip=" + IPAddress, ISRestWorker::QueryType::GET, VariantMap);
}
//-----------------------------------------------------------------------------
void ISDaDataService::SuccessOrganization(const QJsonObject &JsonObject)
{
	QVariantMap VariantMap = JsonObject.toVariantMap();
	QVariant Suggestions = VariantMap["suggestions"];
	QVariantList VariantList = Suggestions.toList();
	if (VariantList.isEmpty())
	{
		emit Error(LANG("DaData.NotFound.Organization"));
	}
	else
	{
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
		Organization.Management.FIO = VariantMap["data"].toMap()["management"].toMap()["name"].toString();
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
		emit FoundedOgranization(Organization);
	}
}
//-----------------------------------------------------------------------------
void ISDaDataService::SuccessBank(const QJsonObject &JsonObject)
{
	QVariantMap VariantMap = JsonObject.toVariantMap();
	QVariant Suggestions = VariantMap["suggestions"];
	QVariantList VariantList = Suggestions.toList();
	if (VariantList.isEmpty())
	{
		emit Error(LANG("DaData.NotFound.Bank"));
	}
	else
	{
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
		emit FoundedBank(Bank);
	}
}
//-----------------------------------------------------------------------------
void ISDaDataService::SuccessLocation(const QJsonObject &JsonObject)
{
	QVariantMap VariantMap = JsonObject.toVariantMap();
	VariantMap = VariantMap["location"].toMap();
	if (VariantMap.isEmpty())
	{
		emit Error(LANG("DaData.NotFound.IPAddress"));
	}
	else
	{
		ISDaDataLocation Location;
		Location.Value = VariantMap["value"].toString();
		Location.ValueFull = VariantMap["unrestricted_value"].toString();
		Location.PostalCode = VariantMap["data"].toMap()["postal_code"].toString();
		Location.Country = VariantMap["data"].toMap()["country"].toString();
		Location.CountryCodeISO = VariantMap["data"].toMap()["country_iso_code"].toString();
		Location.FederalDisctrict = VariantMap["data"].toMap()["federal_district"].toString();
		Location.RegionFiasID = VariantMap["data"].toMap()["region_fias_id"].toString();
		Location.RegionKladrID = VariantMap["data"].toMap()["region_kladr_id"].toString();
		Location.RegionCodeISO = VariantMap["data"].toMap()["region_iso_code"].toString();
		Location.RegionWithType = VariantMap["data"].toMap()["region_with_type"].toString();
		Location.RegionType = VariantMap["data"].toMap()["region_type"].toString();
		Location.RegionTypeFull = VariantMap["data"].toMap()["region_type_full"].toString();
		Location.Region = VariantMap["data"].toMap()["region"].toString();
		Location.AreaFiasID = VariantMap["data"].toMap()["area_fias_id"].toString();
		Location.AreaKladrID = VariantMap["data"].toMap()["area_kladr_id"].toString();
		Location.AreaWithType = VariantMap["data"].toMap()["area_with_type"].toString();
		Location.AreaType = VariantMap["data"].toMap()["area_type"].toString();
		Location.AreaTypeFull = VariantMap["data"].toMap()["area_type_full"].toString();
		Location.Area = VariantMap["data"].toMap()["area"].toString();
		Location.CityFiasID = VariantMap["data"].toMap()["city_fias_id"].toString();
		Location.CityKladrID = VariantMap["data"].toMap()["city_kladr_id"].toString();
		Location.CityWithType = VariantMap["data"].toMap()["city_with_type"].toString();
		Location.CityType = VariantMap["data"].toMap()["city_type"].toString();
		Location.CityTypeFull = VariantMap["data"].toMap()["city_type_full"].toString();
		Location.City = VariantMap["data"].toMap()["city"].toString();
		Location.CityArea = VariantMap["data"].toMap()["city_area"].toString();
		Location.CityDistrictFiasID = VariantMap["data"].toMap()["city_district_fias_id"].toString();
		Location.CityDistrictKladrID = VariantMap["data"].toMap()["city_district_kladr_id"].toString();
		Location.CityDistrictWithType = VariantMap["data"].toMap()["city_district_with_type"].toString();
		Location.CityDistrictType = VariantMap["data"].toMap()["city_district_type"].toString();
		Location.CityDistrictTypeFull = VariantMap["data"].toMap()["city_district_type_full"].toString();
		Location.CityDistrict = VariantMap["data"].toMap()["city_district"].toString();
		Location.SettlementFiasID = VariantMap["data"].toMap()["settlement_fias_id"].toString();
		Location.SettlementKladrID = VariantMap["data"].toMap()["settlement_kladr_id"].toString();
		Location.SettlementWithType = VariantMap["data"].toMap()["settlement_with_type"].toString();
		Location.SettlementType = VariantMap["data"].toMap()["settlement_type"].toString();
		Location.SettlementTypeFull = VariantMap["data"].toMap()["settlement_type_full"].toString();
		Location.Settlement = VariantMap["data"].toMap()["settlement"].toString();
		Location.StreetFiasID = VariantMap["data"].toMap()["street_fias_id"].toString();
		Location.StreetKladrID = VariantMap["data"].toMap()["street_kladr_id"].toString();
		Location.StreetWithType = VariantMap["data"].toMap()["street_with_type"].toString();
		Location.StreetType = VariantMap["data"].toMap()["street_type"].toString();
		Location.StreetTypeFull = VariantMap["data"].toMap()["street_type_full"].toString();
		Location.Street = VariantMap["data"].toMap()["street"].toString();
		Location.HouseFiasID = VariantMap["data"].toMap()["house_fias_id"].toString();
		Location.HouseKladrID = VariantMap["data"].toMap()["house_kladr_id"].toString();
		Location.HouseType = VariantMap["data"].toMap()["house_type"].toString();
		Location.HouseTypeFull = VariantMap["data"].toMap()["house_type_full"].toString();
		Location.House = VariantMap["data"].toMap()["house"].toString();
		Location.BlockType = VariantMap["data"].toMap()["block_type"].toString();
		Location.BlockTypeFull = VariantMap["data"].toMap()["block_type_full"].toString();
		Location.Block = VariantMap["data"].toMap()["block"].toString();
		Location.FlatType = VariantMap["data"].toMap()["flat_type"].toString();
		Location.FlatTypeFull = VariantMap["data"].toMap()["flat_type_full"].toString();
		Location.Flat = VariantMap["data"].toMap()["flat"].toString();
		Location.FlatArea = VariantMap["data"].toMap()["flat_area"].toString();
		Location.SquareMeterPrice = VariantMap["data"].toMap()["square_meter_price"].toString();
		Location.FlatPrice = VariantMap["data"].toMap()["flat_price"].toString();
		Location.PostalBox = VariantMap["data"].toMap()["postal_box"].toString();
		Location.FiasID = VariantMap["data"].toMap()["fias_id"].toString();
		Location.FiasCode = VariantMap["data"].toMap()["fias_code"].toString();
		Location.FiasLevel = VariantMap["data"].toMap()["fias_level"].toString();
		Location.FiasActualityState = VariantMap["data"].toMap()["fias_actuality_state"].toString();
		Location.KladrID = VariantMap["data"].toMap()["kladr_id"].toString();
		Location.GeonameID = VariantMap["data"].toMap()["geoname_id"].toString();
		Location.CapitalMarker = VariantMap["data"].toMap()["capital_marker"].toString();
		Location.Okato = VariantMap["data"].toMap()["okato"].toString();
		Location.Oktmo = VariantMap["data"].toMap()["oktmo"].toString();
		Location.TaxOffice = VariantMap["data"].toMap()["tax_office"].toString();
		Location.TaxOfficeLegal = VariantMap["data"].toMap()["tax_office_legal"].toString();
		Location.Timezone = VariantMap["data"].toMap()["timezone"].toString();
		Location.GeoLat = VariantMap["data"].toMap()["geo_lat"].toString();
		Location.GeoLon = VariantMap["data"].toMap()["geo_lon"].toString();
		Location.BeltwayHit = VariantMap["data"].toMap()["beltway_hit"].toString();
		Location.BeltwayDistance = VariantMap["data"].toMap()["beltway_distance"].toString();
		Location.Metro = VariantMap["data"].toMap()["metro"].toString();
		Location.QCGeo = VariantMap["data"].toMap()["qc_geo"].toString();
		Location.QCComplete = VariantMap["data"].toMap()["qc_complete"].toString();
		Location.QCHouse = VariantMap["data"].toMap()["qc_house"].toString();
		Location.HistoryValues = VariantMap["data"].toMap()["history_values"].toString();
		Location.UnparsedParts = VariantMap["data"].toMap()["unparsed_parts"].toString();
		Location.Source = VariantMap["data"].toMap()["source"].toString();
		Location.QC = VariantMap["data"].toMap()["qc"].toString();
		emit FoundedLocation(Location);
	}
}
//-----------------------------------------------------------------------------
void ISDaDataService::ErrorWorker(const QJsonObject &JsonObject)
{
	emit Error(JsonObject.toVariantMap().value("message").toString());
}
//-----------------------------------------------------------------------------
