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
typedef QMap<QString, QString> QStringMap;
typedef QVector<int> QVectorInt;
typedef QVector<QString> QVectorString;
//-----------------------------------------------------------------------------
