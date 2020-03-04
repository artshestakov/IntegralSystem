#pragma once
#ifndef _ISSTRUCTS_H_INCLUDED
#define _ISSTRUCTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISUuid.h"
#include "ISConstants.h"
#include "ISAssert.h"
//-----------------------------------------------------------------------------
struct ISMetaDataType
{
	QString TypeName;
	ISNamespace::FieldType TypeField;
	QString TypeDB;
	QString ControlWidget;
	QString SearchOperatorWidget;
	bool Search;
};
//-----------------------------------------------------------------------------
struct StructModelItem
{
	QString FieldName;
	QVector<QVariant> Values;
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
public:
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
public:
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
public:
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
public:
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
public:
	ISSortingMetaTable() : ModificationFlag(false) { }

	QString TableName;
	QString FieldName;
	Qt::SortOrder Sorting;
	bool ModificationFlag;
};
//-----------------------------------------------------------------------------
struct ISColumnSizeItem
{
public:
	ISColumnSizeItem() : ModificationFlag(false) { }

	QMap<QString, int> Fields;
	bool ModificationFlag;
};
//-----------------------------------------------------------------------------
struct ISMetaSubSystem
{
public:
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
public:
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
public:
	QVariant OldValue;
	QVariant NewValue;
};
//-----------------------------------------------------------------------------
struct ISPrintMetaReportField
{
public:
	QString ReplaceValue;
	QString FieldQuery;
	QString QueryName;
	QString ParameterName;
};
//-----------------------------------------------------------------------------
struct ISPrintMetaReport
{
public:
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
public:
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
#endif
