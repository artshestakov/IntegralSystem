#pragma once
//-----------------------------------------------------------------------------
#include "libCore_global.h"
#include "ISUuid.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class LIBCORE_EXPORT ISCore
{
public:
	static bool Startup(QString &ErrorString); //Инициализация ядра

	static bool DeleteOrRecoveryObject(ISNamespace::DeleteRecoveryObject DeleteOrRecovery, const QString &TableName, const QString &TableAlias, int ID, const QString &LocalListName); //Удалить/Восстановить запись
	static bool DeleteCascadeObject(const QString &TableName, const QString &TableAlias, int ObjectID); //Удалить объект каскадом

	static void ChangeUser(); //Смена пользователя
	static void RestartApplication(); //Перезапуск программы
	static void ExitApplication(); //Выход из программы

	static QString GetObjectName(PMetaClassTable *MetaTable, int ObjectID); //Получить наименование объекта
	static QString GetVersionInFileName(const QString &FileName); //Получить версию из наименования файла программы IntegralSystem

	static void ExecuteStartCommand(); //Выплнение команды перед запуском программы
	static void ExecuteExitComamnd(); //Выполнение команды перед выходом

	//История
	static void AddHistory(const QString &TableName, const QString &LocalListName, const QString &ObjectName, int ObjectID); //Добавить объект в историю
	static void ClearHistory(); //Очистить историю

	//Календарь
	static int CalendarInsert(const QDateTime &DateTime, const QString &Name, const QVariant &Text, const QString &TableName = QString(), int ObjectID = 0); //Добавить событие в календарь
	static bool CalendarCloseEvent(int CalendarID); //Завершить событие календаря

	//EMail
	static void EMailSend(const QString &Server, int Port, const ISUuid &ConnectionUID, const QString &SenderLogin, const QString &SenderPassword, const QString &SenderName, const QString &RecipientLogin, const QString &Subject, const QString &Message);
	static void EMailSend(const QString &RecipientLogin, const QString &Subject, const QString &Message);

	//СМС
	static void SMSSend(const QString &Login, const QString &Password, const QString &Phone, const QString &Message, const QString &Charset);
	static void SMSSend(const QString &Phone, const QString &Message);

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
