#pragma once
#ifndef _ISSTRUCTS_H_INCLUDED
#define _ISSTRUCTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISUuid.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISTypedefs.h"
#include "ISAlgorithm.h"
#include "ISVersion.h"
//-----------------------------------------------------------------------------
struct ISMetaDataType
{
    QString TypeName; //������������ ����
    ISNamespace::FieldType TypeField; //��� ������� � �������
    QString TypeDB; //��� ������ � ����
    QString ControlWidget; //��� ���� ��������������
    QString SearchOperatorWidget; //��� ���������� �������
    bool Search; //����������� ������ �� ����
};
//-----------------------------------------------------------------------------
struct StructModelItem
{
    QString FieldName;
    ISVectorVariant Values;
    ISNamespace::SearchOperatorType Operator;
    ISNamespace::FieldType FieldType;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISCurrentUserInfo
{
    ISCurrentUserInfo() : System(false), ID(0), GroupID(0), GroupFullAccess(false) { }

    bool System; //���������
    unsigned int ID; //������������� ������������
	QString FIO; //���
    QString Login; //�����
	QString Password; //������ (��������)

	unsigned int GroupID; //������������� ������ � ������� ������� ������������
    bool GroupFullAccess; //������ ������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISMetaSetting
{
    ISMetaSetting() : Type(ISNamespace::FT_Unknown) { }

    ISUuid UID;
    QString Name;
    ISNamespace::FieldType Type;
    QString WidgetEditName;
    QString LocalName;
    QString Hint;
    QVariant Default;
    QVariant Value;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISMetaSettingsGroup
{
    ISMetaSettingsGroup() : System(true) { }
    ~ISMetaSettingsGroup()
    {
        while (!Settings.empty())
        {
			delete ISAlgorithm::VectorTakeBack(Settings);
        }
    }

    QString Name;
    ISUuid UID; //??? ����� �������� �� ����� ��� ���� �� �����������
    QString LocalName;
    QString IconName;
    bool System;
    QString Hint;
    std::vector<ISMetaSetting*> Settings;
};
//-----------------------------------------------------------------------------
struct ISMetaParagraph
{
    ISUuid UID;
    QString Name;
    QString LocalName;
    QString ToolTip;
    QString Icon;
    QString ClassName;
};
//-----------------------------------------------------------------------------
struct ISMetaSubSystem
{
    ISMetaSubSystem() { }

    ISUuid UID; //������������� ����������
    QString LocalName; //��������� ��� ����������
    QString IconName; //��� ������ ����������
    QString TableName; //��� ������� ����������
    QString ClassName; //��� ������ ����������
    QString Hint; //���������

    QString SystemUID; //������������� ������� � ������� ��������� ��� ����������
    QString SystemLocalName; //��� ������������ �������
};
//-----------------------------------------------------------------------------
struct ISMetaSystem
{
    ISMetaSystem() : IsSystem(false) { }

    ~ISMetaSystem()
    {
        while (!SubSystems.empty())
        {
            delete ISAlgorithm::VectorTakeBack(SubSystems);
        }
    }

    bool IsSystem; //��������� ������� (���� ��� - ������ ����������������)
    ISUuid UID; //�������������
    QString LocalName; //��� �������
    QString IconName; //��� ������
    QString Hint; //����������� ���������
    std::vector<ISMetaSubSystem*> SubSystems; //������ ���������
};
//-----------------------------------------------------------------------------
struct ISPrintMetaReport
{
    ISPrintMetaReport() : Type(ISNamespace::RT_Unknown) { }

    void SetType(const QString &type)
    {
        if (type == REPORT_TYPE_HTML)
        {
            Type = ISNamespace::RT_Html;
        }
		else
        {
            IS_ASSERT(false, QString("Inknown report type: %1").arg(type));
        }
    }

	ISUuid UID;
    ISNamespace::ReportType Type;
    QString TableName;
    QString LocalName;
    QString FileTemplate;
    ISStringMap Fields;
};
//-----------------------------------------------------------------------------
struct PMetaUserPermission
{
    PMetaUserPermission() : Show(false), CreateCopy(false), Edit(false), Delete(false), UpdateList(false), ShowDeleted(false),
        ShowAll(false), Search(false), Export(false), Print(false), SystemInformation(false), AttachTask(false), TableNavigation(false) { }

    QString SubSystemUID;
    QString TableUID;

    bool Show;
    bool Create;
    bool CreateCopy;
    bool Edit;
    bool Delete;
    bool UpdateList;
    bool ShowDeleted;
    bool ShowAll;
    bool Search;
    bool Export;
    bool Print;
    bool SystemInformation;
    bool AttachTask;
    bool TableNavigation;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISConfigurationInfo
{
    ISUuid UID; //�������������
    QString Name; //������������
    QString LocalName; //��������� ������������
    QString DesktopForm; //��� ������ �������� �����
	QDate DateExpired; //����, ����� ������� ��������� ����������� �� ������
	QString LogoName;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISSqlQuery
{
    QString FileName; //��� �����
    int Line; //����� ������
    QString SqlText; //SQL-������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISVersionStruct
{
    ISVersion Version;

	QString Date;
	QString Time;
	QString Hash;
	QString Branch;
    QString Configuration;
	QString Platform;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationBranch
{
    ISDaDataOrganizationBranch() : Count(0) { }

    int Count; //���������� ��������
    QString BranchType; //��� �������
    QString BranchTypeLocalName; //��������� ������������ ���� �������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationManagement
{
    QString FIO; //��� ������������
    QString Post; //��������� ������������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationName
{
    QString FullWithOPF; //������ ������������ � ���
    QString ShortWithOPF; //������� ������������ � ���
    QString Full; //������ ������������
    QString Short; //������� ������������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationOkved
{
    ISDaDataOrganizationOkved() : Version(0) { }

    QString Okved; //��� �����
    int Version; //��� ����������� (2001 ��� 2014)
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationOPF
{
    ISDaDataOrganizationOPF() : Type(0) { }

    QString Code; //��� �����
    QString FullName; //������ �������� ���
    QString ShortName; //����������� �������� ���
    int Type; //������ ����������� ����� (1999, 2012 ��� 2014)
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationState
{
    QString ActualityDate; //���� ������������ ��������
    QString RegistrationDate; //���� �����������
    QString LiquidationDate; //���� ����������
    QString StatusName; //������ �����������
    QString StatusLocalName; //��������� ������������ ������� �����������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganization
{
    QString Value; //������������ �����������
    QString Address; //�����
    ISDaDataOrganizationBranch Branch; //������
    QString Inn; //���
    QString Kpp; //���
    QString Ogrn; //����
    QString OrgnDate; //���� ������ ����
    QString HID; //���������� ������������� � DaData
    ISDaDataOrganizationManagement Management; //������������
    ISDaDataOrganizationName Name; //������������ �����������
    ISDaDataOrganizationOkved Okved; //�����
    ISDaDataOrganizationOPF OPF; //��������������-�������� �����
    ISDaDataOrganizationState State; //������ �����������
    QString Type; //��� �����������
    QString TypeLocalName; //��������� ������������ ���� �����������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataBankName
{
    QString Payment; //��������� ������������
    QString Short; //������� ������������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataBankState
{
    QString ActualityDate; //���� ������������ ��������
    QString RegistrationDate; //���� �����������
    QString LiquidationDate; //���� ����������
    QString StatusName; //������ �����������
    QString StatusLocalName; //��������� ������������ ������� �����������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataBank
{
    QString Value; //������������ �����
    QString Address; //�����
    QString BIK; //���������� ����������������� ���
    QString Swift; //���������� ����������������� ��� � ������� SWIFT
    QString RegistrationNumber; //��������������� ����� � �� ��
    QString CorrespondentAccount; //����������������� ����� �� ��
    ISDaDataBankName Name; //������������
    QString PaymentCity; //����� ��� ���������� ���������
    QString OPF; //��� ��������� �����������
    QString OPFLocalName; //��������� ������������ ���� ��������� �����������
    ISDaDataBankState State; //���������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataLocation
{
    QString Value;
    QString ValueFull;
    QString PostalCode;
    QString Country;
    QString CountryCodeISO;
    QString FederalDisctrict;
    QString RegionFiasID;
    QString RegionKladrID;
    QString RegionCodeISO;
    QString RegionWithType;
    QString RegionType;
    QString RegionTypeFull;
    QString Region;
    QString AreaFiasID;
    QString AreaKladrID;
    QString AreaWithType;
    QString AreaType;
    QString AreaTypeFull;
    QString Area;
    QString CityFiasID;
    QString CityKladrID;
    QString CityWithType;
    QString CityType;
    QString CityTypeFull;
    QString City;
    QString CityArea;
    QString CityDistrictFiasID;
    QString CityDistrictKladrID;
    QString CityDistrictWithType;
    QString CityDistrictType;
    QString CityDistrictTypeFull;
    QString CityDistrict;
    QString SettlementFiasID;
    QString SettlementKladrID;
    QString SettlementWithType;
    QString SettlementType;
    QString SettlementTypeFull;
    QString Settlement;
    QString StreetFiasID;
    QString StreetKladrID;
    QString StreetWithType;
    QString StreetType;
    QString StreetTypeFull;
    QString Street;
    QString HouseFiasID;
    QString HouseKladrID;
    QString HouseType;
    QString HouseTypeFull;
    QString House;
    QString BlockType;
    QString BlockTypeFull;
    QString Block;
    QString FlatType;
    QString FlatTypeFull;
    QString Flat;
    QString FlatArea;
    QString SquareMeterPrice;
    QString FlatPrice;
    QString PostalBox;
    QString FiasID;
    QString FiasCode;
    QString FiasLevel;
    QString FiasActualityState;
    QString KladrID;
    QString GeonameID;
    QString CapitalMarker;
    QString Okato;
    QString Oktmo;
    QString TaxOffice;
    QString TaxOfficeLegal;
    QString Timezone;
    QString GeoLat;
    QString GeoLon;
    QString BeltwayHit;
    QString BeltwayDistance;
    QString Metro;
    QString QCGeo;
    QString QCComplete;
    QString QCHouse;
    QString HistoryValues;
    QString UnparsedParts;
    QString Source;
    QString QC;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT CGSectionItem
{
    QString Function;
    QString Description;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT CGSection
{
    QString Name;
    QString ClassName;
    std::vector<CGSectionItem*> Items;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISConnectOptionDB
{
    QString Host; //����� �������
    int Port; //����
    QString Name; //��� ���� ������
    QString Login; //����� ������������
    QString Password; //������
};
//-----------------------------------------------------------------------------
struct ISQueryPoolObject
{
    ISQueryPoolObject(const QString &sql_text) : SqlText(sql_text) { }
    ISQueryPoolObject(const QString &sql_text, const ISStringToVariantMap &parameters) : SqlText(sql_text), Parameters(parameters) { }

    QString SqlText;
    ISStringToVariantMap Parameters;
};
//-----------------------------------------------------------------------------
struct ISHistoryObject
{
	QDateTime DateTime; //���� � ����� ��������
	QString TableName; //��� �������
	int ObjectID; //������������� �������
	bool IsNew; //����� ������
};
//-----------------------------------------------------------------------------
struct ISMessageBoxButton
{
	ISMessageBoxButton(int id, const QString &text, bool _default = false, const QIcon &icon = QIcon()) : ID(id), Text(text), Default(_default), Icon(icon) { }
	ISMessageBoxButton() : ID(-1) { }

	int ID; //������������� ������
	QString Text; //�������
	bool Default; //�� ������ ����� ���������� �����
	QIcon Icon; //������
};
//-----------------------------------------------------------------------------
struct ISConfigParameter
{
	QString TemplateName; //��� �������
	QString Name; //��� ���������
	QVariant::Type Type; //��� ���������
	bool NotNull; //�������� ���������� ��� ����������
	QVariant DefaultValue; //�������� �� ���������
	int Minimum; //����������� �������� (��� �������� ����������)
	int Maximum; //������������ �������� (��� �������� ����������)
};
//-----------------------------------------------------------------------------
struct ISClientInfo
{
	unsigned int ID; //������������� � ��
	QString Address; //IP-�����
	unsigned short Port; //����
};
//-----------------------------------------------------------------------------
struct ISFailAuthInfo
{
	QString IPAddress; //IP-�����
	bool IsLock; //���� ����������
	std::vector<QDateTime> DTPoints; //��������� �����
	QDateTime DTUnlock; //���� � ����� �������������
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISModelField
{
	ISModelField() : IsForeign(false) { }

	QString Name;
	QString LocalName;
	ISNamespace::FieldType Type;
	bool IsForeign;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISModelRecord
{
	ISModelRecord() : ID(0) { }

	unsigned int ID; //������������� ������
	ISVectorVariant Values; //������ ��������
};
//-----------------------------------------------------------------------------
#endif
