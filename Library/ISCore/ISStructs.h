#pragma once
#ifndef _ISSTRUCTS_H_INCLUDED
#define _ISSTRUCTS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISNamespace.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISTypedefs.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
struct ISMetaType
{
	ISMetaType(const QString& type_name, ISNamespace::FieldType type, const QString& type_db, const QString& control_widget, const QString& search_condition_widget, bool search_allowed)
		: TypeName(type_name), TypeField(type), TypeDB(type_db), ControlWidget(control_widget), SearchConditionWidget(search_condition_widget), SearchAllowed(search_allowed)
	{ }

	QString TypeName; //Наименование типа
	ISNamespace::FieldType TypeField; //Тип даннных в системе
	QString TypeDB; //Тип данных в базе
	QString ControlWidget; //Имя поля редактирования
	QString SearchConditionWidget; //Имя поискового виджета
	bool SearchAllowed; //Возможность поиска по полю
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
	ISMetaSubSystem() { }

	QString UID; //Идентификатор подсистемы
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
	QString UID; //Идентификатор
	QString LocalName; //Имя системы
	QString IconName; //Имя иконки
	QString Hint; //Всплывающая подсказка
	std::vector<ISMetaSubSystem*> SubSystems; //Список подсистем
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
struct ISCORE_EXPORT ISConfigurationInfo
{
	QString Name; //Наименование
	QString UID; //Идентификатор
	QString LocalName; //Локальное наименование
	QString DesktopForm; //Имя класса рабочего стола
	QDate DateExpired; //Дата, после которой программа запускаться не должна
	QString LogoName;
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISBuildInfoStruct
{
	ISBuildInfoStruct() : Version(0) { }

	unsigned int Version; //Версия
	QString Date; //Дата сборки
	QString Time; //Время сборки
	QString Hash; //Хэш коммита
	QString BranchName; //Имя ветки
	QString Configuration; //Конфигурация
	QString Platform; //Платформа
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
	ISConnectOptionDB() : Port(0) { }

	ISConnectOptionDB(const QString& host, unsigned short port, const QString& name, const QString& login, const QString& password)
		: Host(host), Port(port), Name(name), Login(login), Password(password) { }

	QString Host; //Адрес сервера
	unsigned short Port; //Порт
	QString Name; //Имя базы данных
	QString Login; //Логин пользователя
	QString Password; //Пароль
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
	ISMessageBoxButton(int id, const QString& text, bool _default = false, const QIcon& icon = QIcon()) : ID(id), Text(text), Default(_default), Icon(icon) { }
	//ISMessageBoxButton() : ID(-1) { } //???

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
	ISClientInfo()
		: ID(0),
		Port(0),
		DTConnected(QDateTime::currentDateTime()),
		LastQueryType(API_UNKNOWN),
		LastQueryResult(false),
		LastQueryMSec(0)
	{ }

	unsigned int ID; //Идентификатор в БД
	QString Address; //IP-адрес
	unsigned short Port; //Порт
	QDateTime DTConnected; //Дата и время подключения
	QDateTime LastQueryDT; //Дата и время последнего запроса
	QString LastQueryType; //Тип последнего запроса
	bool LastQueryResult; //Результат выполнения последнего запроса
	unsigned long long LastQueryMSec; //Время выполнения последнего запроса
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
	ISModelField() : Index(0), Type(ISNamespace::FieldType::Unknown), IsForeign(false), IsSystem(false) { }

	size_t Index; //Индекс
	QString Name; //Имя
	QString LocalName; //Локальное имя
	ISNamespace::FieldType Type; //Тип
	bool IsForeign; //Флаг внешнего ключа
	bool IsSystem; //Флаг системного поля
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISModelRecord
{
	ISModelRecord() : ID(0) { }

	unsigned int ID; //Идентификатор записи
	ISVectorVariant Values; //Список значений
};
//-----------------------------------------------------------------------------
struct ISCORE_EXPORT ISSqlField
{
	std::string Name; //Имя поля
	unsigned int Type;
};
//-----------------------------------------------------------------------------
#endif
