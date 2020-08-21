#pragma once
//-----------------------------------------------------------------------------
#include "iscore_global.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISCORE_EXPORT ISCore
{
public:
	static bool Startup(bool IsGui, const QString &ConfigTemplateName, QString &ErrorString); //Инициализация ядра
	static void ExitApplication(); //Выход из программы
	static QString GetObjectName(const QString &TableName, int ObjectID); //Получить наименование объекта
	static QString GetObjectName(PMetaTable *MetaTable, int ObjectID); //Получить наименование объекта

	//История
	static void AddHistory(const QString &TableName, const QString &LocalListName, const QString &ObjectName, int ObjectID); //Добавить объект в историю
	static void ClearHistory(); //Очистить историю

	//Календарь
	static int CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName = QString(), int ObjectID = 0); //Добавить событие в календарь
	static bool CalendarCloseEvent(int CalendarID); //Завершить событие календаря

	//Задачи
	static bool TaskCheckExist(int TaskID); //Проверить существование задачи по её номеру

	//Объекты
	static bool SetIsDeletedObject(bool IsDeleted, PMetaTable *MetaTable, int ObjectID, QString &ErrorString); //Удалить/восстановить объект
	static bool SetIsDeletedObjects(bool IsDeleted, PMetaTable *MetaTable, const ISVectorInt &Objects, QString &ErrorString); //Удалить/восстановить объекты
	static bool DeleteCascadeObject(PMetaTable *MetaTable, int ObjectID, QString &ErrorString); //Удалить объект каскадом
};
//-----------------------------------------------------------------------------
