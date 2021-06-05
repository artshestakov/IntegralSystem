#pragma once
#ifndef _ISSTRUCTSOLD_H_INCLUDED
#define _ISSTRUCTSOLD_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISConstants.h"
#include "ISConstantsOld.h"
#include "ISAssert.h"
#include "ISTypedefsOld.h"
#include "ISAlgorithmOld.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
struct ISCurrentUserInfo
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
struct ISMetaSetting
{
	ISMetaSetting() : Type(ISNamespace::FieldType::Unknown) { }

	QString UID;
	QString Name;
	ISNamespace::FieldType Type;
	QString WidgetEditName;
	QString LocalName;
	QString Hint;
	QVariant Default;
	QVariant Value;
};
//-----------------------------------------------------------------------------
struct ISMetaSettingsGroup
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
	QString UID;
	QString LocalName;
	QString IconName;
	bool System;
	QString Hint;
	std::vector<ISMetaSetting*> Settings;
};
//-----------------------------------------------------------------------------
struct ISMetaParagraph
{
	QString UID;
	QString Name;
	QString LocalName;
	QString ToolTip;
	QString Icon;
	QString ClassName;
};
//-----------------------------------------------------------------------------
struct ISMetaSubSystem
{
	QString UID; //������������� ����������
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
	QString UID; //�������������
	QString LocalName; //��� �������
	QString IconName; //��� ������
	QString Hint; //����������� ���������
	std::vector<ISMetaSubSystem*> SubSystems; //������ ���������
};
//-----------------------------------------------------------------------------
struct PMetaUserPermission
{
	PMetaUserPermission() : Show(false), Create(false), CreateCopy(false), Edit(false), Delete(false), UpdateList(false), ShowDeleted(false),
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
struct ISBuildInfoStruct
{
	ISBuildInfoStruct() : Version(0) { }

	unsigned int Version; //������
	QString Date; //���� ������
	QString Time; //����� ������
	QString Hash; //��� �������
	QString BranchName; //��� �����
	QString Configuration; //������������
	QString Platform; //���������
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
	ISMessageBoxButton(int id, const QString& text, bool _default = false, const QIcon& icon = QIcon()) : ID(id), Text(text), Default(_default), Icon(icon) { }
	//ISMessageBoxButton() : ID(-1) { } //???

	int ID; //������������� ������
	QString Text; //�������
	bool Default; //�� ������ ����� ���������� �����
	QIcon Icon; //������
};
//-----------------------------------------------------------------------------
struct ISConfigParameter2
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
	ISClientInfo()
		: ID(0),
		Port(0),
		DTConnected(QDateTime::currentDateTime()),
		LastQueryType(API_UNKNOWN),
		LastQueryResult(false),
		LastQueryMSec(0)
	{ }

	unsigned int ID; //������������� � ��
	QString Address; //IP-�����
	unsigned short Port; //����
	QDateTime DTConnected; //���� � ����� �����������
	QDateTime LastQueryDT; //���� � ����� ���������� �������
	QString LastQueryType; //��� ���������� �������
	bool LastQueryResult; //��������� ���������� ���������� �������
	unsigned long long LastQueryMSec; //����� ���������� ���������� �������
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
struct ISModelField
{
	ISModelField() : Index(0), Type(ISNamespace::FieldType::Unknown), IsForeign(false), IsSystem(false) { }

	size_t Index; //������
	QString Name; //���
	QString LocalName; //��������� ���
	ISNamespace::FieldType Type; //���
	bool IsForeign; //���� �������� �����
	bool IsSystem; //���� ���������� ����
};
//-----------------------------------------------------------------------------
struct ISModelRecord
{
	ISModelRecord() : ID(0) { }

	unsigned int ID; //������������� ������
	ISVectorVariant Values; //������ ��������
};
//-----------------------------------------------------------------------------
struct ISSqlField
{
	std::string Name; //��� ����
	unsigned int Type;
};
//-----------------------------------------------------------------------------
#endif