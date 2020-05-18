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
	static QString GetObjectName(PMetaTable *MetaTable, int ObjectID); //Получить наименование объекта

	//История
	static void AddHistory(const QString &TableName, const QString &LocalListName, const QString &ObjectName, int ObjectID); //Добавить объект в историю
	static void ClearHistory(); //Очистить историю

	//Календарь
	static int CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName = QString(), int ObjectID = 0); //Добавить событие в календарь
	static bool CalendarCloseEvent(int CalendarID); //Завершить событие календаря

	//Задачи
	static ISUuid TaskGetStatusUID(int TaskID); //Получить статус задачи по её идентификатору
	static void TaskSetStatus(int TaskID, const ISUuid &StatusUID, const QVariant &Resolution = QVariant()); //Обновиь статус задачи
	static bool TaskCheckExist(int TaskID); //Проверить существование задачи по её номеру
	static bool TaskIsDuplicate(int TaskOriginalID, int TaskDuplicateID); //Проверить является ли задача дубликатом
	static void TaskInsertDuplicate(int TaskOriginalID, int TaskDuplicateID); //Отметить задачу как дубликат
	static void TaskInsertHistory(int TaskID, int UserID, const ISUuid &HistoryUID, const QString &Information); //Добавить действие в историю задачи
	static void TaskInsertHistory(int TaskID, const ISUuid &HistoryUID, const QString &Information); //Добавить действие в историю задачи
	static bool TaskIsAttachedObject(int TaskID, const QString &TableName, int ObjectID); //Проверить не прикреплен ли объект уже к задаче
	static bool TaskAttachObject(int TaskID, const QString &TableName, int ObjectID); //Прикрепить запись к задаче
	static bool TaskDetachObject(int TaskID, const QString &TableName, int ObjectID); //Открепить запись от задачи
	static int TaskCountOverdue(); //Получить количество просроченных задач
};
//-----------------------------------------------------------------------------
