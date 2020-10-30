#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCore
{
public:
	static QString GetObjectName(const QString &TableName, int ObjectID); //Получить наименование объекта
	static QString GetObjectName(PMetaTable *MetaTable, int ObjectID); //Получить наименование объекта

	//Календарь
	static int CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName = QString(), int ObjectID = 0); //Добавить событие в календарь
	static bool CalendarCloseEvent(int CalendarID); //Завершить событие календаря

	//Задачи
	static bool TaskCheckExist(int TaskID); //Проверить существование задачи по её номеру

	//Объекты
	static bool SetIsDeletedObject(bool IsDeleted, PMetaTable *MetaTable, int ObjectID, QString &ErrorString); //Удалить/восстановить объект
	static bool SetIsDeletedObjects(bool IsDeleted, PMetaTable *MetaTable, const ISVectorInt &Objects, QString &ErrorString); //Удалить/восстановить объекты
	static bool DeleteCascadeObject(PMetaTable *MetaTable, int ObjectID, QString &ErrorString); //Удалить объект каскадом

	//Номера телефонов
	static void PhoneNumberPrepare(QString &PhoneNumber); //Подготовить номер телефона (удалить все кроме цифр)
	static void PhoneNumberFormat(QString &PhoneNumber); //Форматировать номер телефона
};
//-----------------------------------------------------------------------------
