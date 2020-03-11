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
	QString TypeName; //Наименование типа
	ISNamespace::FieldType TypeField; //Тип даннных в системе
	QString TypeDB; //Тип данных в базе
	QString ControlWidget; //Имя поля редактирования
	QString SearchOperatorWidget; //Имя поискового виджета
	bool Search; //Возможность поиска по полю
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

	bool System; //Системный
	int ID; //Идентификатор пользователя
	QString Surname; //Фамилия
	QString Name; //Имя
	QString Patronymic; //Отчество
	QDate Birthday; //Дата рождения
	QString Login; //Логин
	QString Password; //Пароль
	QString IPAddress; //IP-адресс
	QString FullName; //ФИО
	bool AccessAllowed; //Доступ разрешен
	bool AccountLifetime; //Использовать срок действия учётной записи
	QDate AccountLifetimeStart; //Начало действия учётной записи
	QDate AccountLifetimeEnd; //Окончание действия учётной записи
	bool FixedInactive; //Фиксировать бездействие
	int InactiveTimeout; //Таймаут фиксирования бездействия

	int GroupID; //Идентификатор группы в которой состоит пользователь
	QString GroupName; //Наименование группы пользователя
	bool GroupFullAccess; //Полный доступ
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

	int ID; //Идентификатор подсистемы
	ISUuid UID; //Идентификатор подсистемы
	QString LocalName; //Локальное имя подсистемы
	int OrderID; //Порядок подсистемы
	QString IconName; //Имя иконки подсистемы
	QString TableName; //Имя таблицы подсистемы
	QString ClassName; //Имя класса подсистемы
	QString Hint; //Подсказка

	QString SystemUID; //Идентификатор системы к которой относится эта подсистема
	int SystemID; //Идентификатор родительской системы
	QString SystemLocalName; //Имя родительской системы
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

	bool IsSystem; //Движковая система (если нет - значит пользовательская)
	int ID;
	ISUuid UID; //Идентификатор
	QString LocalName; //Имя системы
	int OrderID; //Порядок системы
	QString IconName; //Имя иконки
	QByteArray IconImage; //Пользовательская иконка
	QString Hint;

	QVector<ISMetaSubSystem*> SubSystems; //Список подсистем
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

	bool IsValid; //Валидность конфигурации
	ISUuid UID; //Идентификатор
	QString Name; //Наименование
	QString LocalName; //Локальное наименование
	QString ClassName; //Имя класса ядра конфигурации
	QString DesktopForm; //Имя класса рабочего стола
	QString IncomingCallForm; //Имя класса формы входящего звонка
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISSqlQuery
{
	QString FileName; //Имя файла
	int Line; //Номер строки
	QString SqlText; //SQL-запрос
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

	int Count; //Количество филиалов
	QString BranchType; //Тип филиала
	QString BranchTypeLocalName; //Локальное наименование типа филиала
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationManagement
{
	QString Name; //ФИО руководителя
	QString Post; //Должность руководителя
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationName
{
	QString FullWithOPF; //Полное наименование с ОПФ
	QString ShortWithOPF; //Краткое наименование с ОПФ
	QString Full; //Полное наименование
	QString Short; //Краткое наименование
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationOkved
{
	ISDaDataOrganizationOkved() : Version(0) { }

	QString Okved; //Код ОКВЭД
	int Version; //Тип справочника (2001 или 2014)
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationOPF
{
	ISDaDataOrganizationOPF() : Type(0) { }

	QString Code; //Код ОКОПФ
	QString FullName; //Полное название ОПФ
	QString ShortName; //Сокращенное название ОПФ
	int Type; //Версия справочника ОКОПФ (1999, 2012 или 2014)
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganizationState
{
	QString ActualityDate; //Дата актуальности сведений
	QString RegistrationDate; //Дата регистрации
	QString LiquidationDate; //Дата ликвидации
	QString StatusName; //Статус организации
	QString StatusLocalName; //Локальное наименование статуса организации
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataOrganization
{
	QString Value; //Наименование организации
	QString Address; //Адрес
	ISDaDataOrganizationBranch Branch; //Филиал
	QString Inn; //ИНН
	QString Kpp; //КПП
	QString Ogrn; //ОГРН
	QString OrgnDate; //Дата выдачи ОГРН
	QString HID; //Уникальный идентификатор в DaData
	ISDaDataOrganizationManagement Management; //Руководитель
	ISDaDataOrganizationName Name; //Наименование организации
	ISDaDataOrganizationOkved Okved; //ОКВЭД
	ISDaDataOrganizationOPF OPF; //Организационно-правовая форма
	ISDaDataOrganizationState State; //Статус огранизации
	QString Type; //Тип организации
	QString TypeLocalName; //Локальное наименование типа огранизации
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataBankName
{
	QString Payment; //Платежное наименование
	QString Short; //Краткое наименование
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataBankState
{
	QString ActualityDate; //Дата актуальности сведений
	QString RegistrationDate; //Дата регистрации
	QString LiquidationDate; //Дата ликвидации
	QString StatusName; //Статус организации
	QString StatusLocalName; //Локальное наименование статуса организации
};
//-----------------------------------------------------------------------------
struct LIBCORE_EXPORT ISDaDataBank
{
	QString Value; //Наименование банка
	QString Address; //Адрес
	QString BIK; //Банковский идентификационный код
	QString Swift; //Банковский идентификационный код в системе SWIFT
	QString RegistrationNumber; //Регистрационный номер в ЦБ РФ
	QString CorrespondentAccount; //Корреспондентский счётв ЦБ РФ
	ISDaDataBankName Name; //Наименование
	QString PaymentCity; //Город для платежного поручения
	QString OPF; //Тип кредитной организации
	QString OPFLocalName; //Локальное наименование типа кредитной организации
	ISDaDataBankState State; //Состояние
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
