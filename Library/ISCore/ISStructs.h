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
struct ISCORE_EXPORT ISCurrentUserInfo
{
    ISCurrentUserInfo() : System(false), ID(0), GroupID(0), GroupFullAccess(false) { }

    bool System; //Системный
    unsigned int ID; //Идентификатор пользователя
	QString FIO; //ФИО
    QString Login; //Логин
	QString Password; //Пароль (временно)

	unsigned int GroupID; //Идентификатор группы в которой состоит пользователь
    bool GroupFullAccess; //Полный доступ
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
    ISUuid UID; //??? После перехода на Карат это поле не понадобится
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

    ISUuid UID; //Идентификатор подсистемы
    QString LocalName; //Локальное имя подсистемы
    QString IconName; //Имя иконки подсистемы
    QString TableName; //Имя таблицы подсистемы
    QString ClassName; //Имя класса подсистемы
    QString Hint; //Подсказка

    QString SystemUID; //Идентификатор системы к которой относится эта подсистема
    QString SystemLocalName; //Имя родительской системы
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

    bool IsSystem; //Движковая система (если нет - значит пользовательская)
    ISUuid UID; //Идентификатор
    QString LocalName; //Имя системы
    QString IconName; //Имя иконки
    QString Hint; //Всплывающая подсказка
    std::vector<ISMetaSubSystem*> SubSystems; //Список подсистем
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
    ISUuid UID; //Идентификатор
    QString Name; //Наименование
    QString LocalName; //Локальное наименование
    QString DesktopForm; //Имя класса рабочего стола
	QDate DateExpired; //Дата, после которой программа запускаться не должна
	QString LogoName;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISSqlQuery
{
    QString FileName; //Имя файла
    int Line; //Номер строки
    QString SqlText; //SQL-запрос
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

    int Count; //Количество филиалов
    QString BranchType; //Тип филиала
    QString BranchTypeLocalName; //Локальное наименование типа филиала
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationManagement
{
    QString FIO; //ФИО руководителя
    QString Post; //Должность руководителя
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationName
{
    QString FullWithOPF; //Полное наименование с ОПФ
    QString ShortWithOPF; //Краткое наименование с ОПФ
    QString Full; //Полное наименование
    QString Short; //Краткое наименование
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationOkved
{
    ISDaDataOrganizationOkved() : Version(0) { }

    QString Okved; //Код ОКВЭД
    int Version; //Тип справочника (2001 или 2014)
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationOPF
{
    ISDaDataOrganizationOPF() : Type(0) { }

    QString Code; //Код ОКОПФ
    QString FullName; //Полное название ОПФ
    QString ShortName; //Сокращенное название ОПФ
    int Type; //Версия справочника ОКОПФ (1999, 2012 или 2014)
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganizationState
{
    QString ActualityDate; //Дата актуальности сведений
    QString RegistrationDate; //Дата регистрации
    QString LiquidationDate; //Дата ликвидации
    QString StatusName; //Статус организации
    QString StatusLocalName; //Локальное наименование статуса организации
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataOrganization
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
struct ISCORE_EXPORT ISDaDataBankName
{
    QString Payment; //Платежное наименование
    QString Short; //Краткое наименование
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataBankState
{
    QString ActualityDate; //Дата актуальности сведений
    QString RegistrationDate; //Дата регистрации
    QString LiquidationDate; //Дата ликвидации
    QString StatusName; //Статус организации
    QString StatusLocalName; //Локальное наименование статуса организации
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISDaDataBank
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
    QString Host; //Адрес сервера
    int Port; //Порт
    QString Name; //Имя базы данных
    QString Login; //Логин пользователя
    QString Password; //Пароль
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
	QDateTime DateTime; //Дата и время открытия
	QString TableName; //Имя таблицы
	int ObjectID; //Идентификатор объекта
	bool IsNew; //Новая запись
};
//-----------------------------------------------------------------------------
struct ISMessageBoxButton
{
	ISMessageBoxButton(int id, const QString &text, bool _default = false, const QIcon &icon = QIcon()) : ID(id), Text(text), Default(_default), Icon(icon) { }
	ISMessageBoxButton() : ID(-1) { }

	int ID; //Идентификатор кнопки
	QString Text; //Надпись
	bool Default; //На кнопке будет установлен фокус
	QIcon Icon; //Иконка
};
//-----------------------------------------------------------------------------
struct ISConfigParameter
{
	QString TemplateName; //Имя шаблона
	QString Name; //Имя параметра
	QVariant::Type Type; //Тип параметра
	bool NotNull; //Параметр обязателен для заполнения
	QVariant DefaultValue; //Значение по умолчанию
	int Minimum; //Минимальное значение (для числовых параметров)
	int Maximum; //Максимальное значение (для числовых параметров)
};
//-----------------------------------------------------------------------------
struct ISClientInfo
{
	unsigned int ID; //Идентификатор в БД
	QString Address; //IP-адрес
	unsigned short Port; //Порт
};
//-----------------------------------------------------------------------------
struct ISFailAuthInfo
{
	QString IPAddress; //IP-адрес
	bool IsLock; //Флаг блокировки
	std::vector<QDateTime> DTPoints; //Временные точки
	QDateTime DTUnlock; //Дата и время разблокировки
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

	unsigned int ID; //Идентификатор записи
	ISVectorVariant Values; //Список значений
};
//-----------------------------------------------------------------------------
#endif
