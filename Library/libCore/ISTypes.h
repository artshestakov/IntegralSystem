#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISUuid.h"
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
struct ISRangeStruct
{
public:
	ISRangeStruct()
	{

	}

	ISRangeStruct(const QVariant &begin_value, const QVariant &end_value)
		: BeginValue(begin_value),
		EndValue(end_value)
	{

	}

	void Clear()
	{
		BeginValue.clear();
		EndValue.clear();
	}

	bool IsValid() const
	{
		return BeginValue.isValid() && EndValue.isValid();
	}

	QVariant BeginValue;
	QVariant EndValue;
};
//-----------------------------------------------------------------------------
struct ISLicenseItem
{
	ISUuid UID;
	QString Name;
	QString LocalName;
	QString ClassName;
	QString DesktopForm;
	QString IncomingCallForm;
};
//-----------------------------------------------------------------------------
struct ISMetaUserData
{
	ISMetaUserData()
		: System(false),
		ID(0),
		GroupID(0),
		AccessAllowed(false),
		FixedInactive(false),
		InactiveTimeout(0),
		GroupFullAccess(false)
	{

	}

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
typedef QMap<QString, QString> QStringMap;
typedef QVector<int> QVectorInt;
typedef QVector<QString> QVectorString;
//-----------------------------------------------------------------------------
