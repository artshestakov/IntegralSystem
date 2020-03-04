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
public:
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
#endif
