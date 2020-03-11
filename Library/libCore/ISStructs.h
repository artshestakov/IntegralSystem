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
struct ISMetaUserData
{
	ISMetaUserData() : System(false), ID(0), GroupID(0), AccessAllowed(false), FixedInactive(false), InactiveTimeout(0), GroupFullAccess(false) { }

	bool System; //���������
	int ID; //������������� ������������
	QString Surname; //�������
	QString Name; //���
	QString Patronymic; //��������
	QDate Birthday; //���� ��������
	QString Login; //�����
	QString Password; //������
	QString IPAddress; //IP-������
	QString FullName; //���
	bool AccessAllowed; //������ ��������
	bool AccountLifetime; //������������ ���� �������� ������� ������
	QDate AccountLifetimeStart; //������ �������� ������� ������
	QDate AccountLifetimeEnd; //��������� �������� ������� ������
	bool FixedInactive; //����������� �����������
	int InactiveTimeout; //������� ������������ �����������

	int GroupID; //������������� ������ � ������� ������� ������������
	QString GroupName; //������������ ������ ������������
	bool GroupFullAccess; //������ ������
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISMetaNotify
{
	ISMetaNotify() : ShowPopup(true) { }

	ISUuid UID;
	QString Name;
	QString SoundFileName;
	QString MessageNotify;
	QString SignalName;
	bool ShowPopup;
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISMetaSetting
{
	ISMetaSetting() : SettingType(ISNamespace::FT_Unknown) { }

	ISUuid UID;
	QString Name;
	ISNamespace::FieldType SettingType;
	QString WidgetEditName;
	QString LocalName;
	QString Hint;
	QVariant DefaultValue;
	QVariant Value;
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISMetaSettingsGroup
{
	ISMetaSettingsGroup() : System(true) { }
	~ISMetaSettingsGroup()
	{
		while (!Settings.isEmpty())
		{
			delete Settings.takeLast();
		}
	}

	QString Name;
	ISUuid UID;
	QString LocalName;
	QString IconName;
	bool System;
	QString Hint;
	QVector<ISMetaSetting*> Settings;
};
//-----------------------------------------------------------------------------
struct ISMetaExternalTool
{
	QString LocalName;
	QString Command;
	ISUuid UID;
	QIcon Icon;
};
//-----------------------------------------------------------------------------
struct ISMetaParagraph
{
	ISMetaParagraph(const ISUuid &uid, const QString &name, const QString &local_name, const QString &tool_tip, const QString &icon, const QString &class_name, bool default)
		: UID(uid), Name(name), LocalName(local_name), ToolTip(tool_tip), Icon(icon), ClassName(class_name), Default(default) { }

	ISUuid UID;
	QString Name;
	QString LocalName;
	QString ToolTip;
	QString Icon;
	QString ClassName;
	bool Default;
};
//-----------------------------------------------------------------------------
struct ISSortingMetaTable
{
	ISSortingMetaTable() : ModificationFlag(false) { }

	QString TableName;
	QString FieldName;
	Qt::SortOrder Sorting;
	bool ModificationFlag;
};
//-----------------------------------------------------------------------------
struct ISColumnSizeItem
{
	ISColumnSizeItem() : ModificationFlag(false) { }

	ISStringToIntMap Fields;
	bool ModificationFlag;
};
//-----------------------------------------------------------------------------
struct ISMetaSubSystem
{
	ISMetaSubSystem() : ID(0), OrderID(0), SystemID(0) { }

	int ID; //������������� ����������
	ISUuid UID; //������������� ����������
	QString LocalName; //��������� ��� ����������
	int OrderID; //������� ����������
	QString IconName; //��� ������ ����������
	QString TableName; //��� ������� ����������
	QString ClassName; //��� ������ ����������
	QString Hint; //���������

	QString SystemUID; //������������� ������� � ������� ��������� ��� ����������
	int SystemID; //������������� ������������ �������
	QString SystemLocalName; //��� ������������ �������
};
//-----------------------------------------------------------------------------
struct ISMetaSystem
{
	ISMetaSystem() : IsSystem(false), ID(0), OrderID(0) { }

	~ISMetaSystem()
	{
		while (!SubSystems.isEmpty())
		{
			delete SubSystems.takeLast();
		}
	}

	bool IsSystem; //��������� ������� (���� ��� - ������ ����������������)
	int ID;
	ISUuid UID; //�������������
	QString LocalName; //��� �������
	int OrderID; //������� �������
	QString IconName; //��� ������
	QByteArray IconImage; //���������������� ������
	QString Hint;

	QVector<ISMetaSubSystem*> SubSystems; //������ ���������
};
//-----------------------------------------------------------------------------
struct ISObjectModelItem
{
	QVariant OldValue;
	QVariant NewValue;
};
//-----------------------------------------------------------------------------
struct ISPrintMetaReportField
{
	QString ReplaceValue;
	QString FieldQuery;
	QString QueryName;
	QString ParameterName;
};
//-----------------------------------------------------------------------------
struct ISPrintMetaReport
{
	ISPrintMetaReport() : Type(ISNamespace::RT_Unknown) { }

	~ISPrintMetaReport()
	{
		while (!Fields.isEmpty())
		{
			delete Fields.takeLast();
		}
	}

	void SetType(const QString &type)
	{
		if (type == REPORT_TYPE_HTML)
		{
			Type = ISNamespace::RT_Html;
		}
		else if (type == REPORT_TYPE_WORD)
		{
			Type = ISNamespace::RT_Word;
		}
		else
		{
			IS_ASSERT(false, QString("Inknown report type: %1").arg(type));
		}
	}

	bool System;
	ISNamespace::ReportType Type;
	QString TableName;
	QString Name;
	QString LocalName;
	QString FileTemplate;

	QVector<ISPrintMetaReportField*> Fields;
};
//-----------------------------------------------------------------------------
struct PMetaUserPermission
{
	PMetaUserPermission() : Show(false), CreateCopy(false), Edit(false), Delete(false), DeleteCascade(false), UpdateList(false), ShowDeleted(false),
		ShowAll(false), Search(false), Export(false), Print(false), SystemInformation(false), AttachTask(false), TableNavigation(false) { }

	QString SubSystemUID;
	QString TableUID;

	bool Show;
	bool Create;
	bool CreateCopy;
	bool Edit;
	bool Delete;
	bool DeleteCascade;
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
struct LIBCORE_EXPORT ISConfigurationItem
{
	ISConfigurationItem() : IsValid(false) { }

	bool IsValid; //���������� ������������
	ISUuid UID; //�������������
	QString Name; //������������
	QString LocalName; //��������� ������������
	QString ClassName; //��� ������ ���� ������������
	QString DesktopForm; //��� ������ �������� �����
	QString IncomingCallForm; //��� ������ ����� ��������� ������
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISSqlQuery
{
	QString FileName; //��� �����
	int Line; //����� ������
	QString SqlText; //SQL-������
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISVersionStruct
{
	ISVersionStruct() : Major(0), Minor(0), Revision(0) { }

	int Major;
	int Minor;
	int Revision;

	QString Date;
	QString Time;
	QString Hash;
	QString Branch;
	QString Configuration;
	QString Platform;
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationBranch
{
	ISDaDataOrganizationBranch() : Count(0) { }

	int Count; //���������� ��������
	QString BranchType; //��� �������
	QString BranchTypeLocalName; //��������� ������������ ���� �������
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationManagement
{
	QString Name; //��� ������������
	QString Post; //��������� ������������
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationName
{
	QString FullWithOPF; //������ ������������ � ���
	QString ShortWithOPF; //������� ������������ � ���
	QString Full; //������ ������������
	QString Short; //������� ������������
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationOkved
{
	ISDaDataOrganizationOkved() : Version(0) { }

	QString Okved; //��� �����
	int Version; //��� ����������� (2001 ��� 2014)
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationOPF
{
	ISDaDataOrganizationOPF() : Type(0) { }

	QString Code; //��� �����
	QString FullName; //������ �������� ���
	QString ShortName; //����������� �������� ���
	int Type; //������ ����������� ����� (1999, 2012 ��� 2014)
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationState
{
	QString ActualityDate; //���� ������������ ��������
	QString RegistrationDate; //���� �����������
	QString LiquidationDate; //���� ����������
	QString StatusName; //������ �����������
	QString StatusLocalName; //��������� ������������ ������� �����������
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganization
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
struct LIBCORE_EXPORT ISDaDataBankName
{
	QString Payment; //��������� ������������
	QString Short; //������� ������������
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataBankState
{
	QString ActualityDate; //���� ������������ ��������
	QString RegistrationDate; //���� �����������
	QString LiquidationDate; //���� ����������
	QString StatusName; //������ �����������
	QString StatusLocalName; //��������� ������������ ������� �����������
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataBank
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
struct LIBCORE_EXPORT ISDaDataLocation
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
struct LIBCORE_EXPORT CGSectionItem
{
	QString Function;
	QString LocalName;
	QString Description;
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT CGSection
{
	QString Name;
	QString ClassName;
	std::vector<CGSectionItem*> Items;
};
//-----------------------------------------------------------------------------
#endif
