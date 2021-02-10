#pragma once
#ifndef _ISTCPWORKER_H_INCLUDED
#define _ISTCPWORKER_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTcpMessage.h"
#include "ISTcpAnswer.h"
#include "ISTypedefs.h"
#include "ISStructs.h"
#include "ISQuery.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISTcpWorker : public QObject
{
	Q_OBJECT

signals:
	void StartedDone();
	void StartedFailed();
	void Answer(ISTcpAnswer *);

public:
	Q_INVOKABLE ISTcpWorker();
	virtual ~ISTcpWorker();

	bool GetStarted() const; //Получить флаг успешного запуска воркера
	bool GetRunning(); //Получить флаг занятости
	void SetDB(const QString &db_host, int db_port, const QString &db_name, const QString &db_user, const QString &db_password);
	void SetMessage(ISTcpMessage *TcpMessage); //Установить сообщение на обработку
	void Run(); //Запуск воркера
	void Stop(); //Остановка воркера

private:
	void Process();
	void Finish(); //Уведомление о завершении работы воркера
	void Protocol(unsigned int UserID, const ISUuid &ActionTypeUID, const QVariant &TableName = QVariant(), const QVariant &TableLocalName = QVariant(), const QVariant &ObjectID = QVariant(), const QVariant &Information = QVariant()); //Протоколирование действия
	bool UserPasswordExist(const QVariant &UserID, bool &Exist); //Проверка наличия пароля у пользователя
	bool UserIsSystem(const QVariant &UserID, bool &IsSystem); //Проверка пользователя на системность
	bool GetObjectName(PMetaTable *MetaTable, unsigned int ObjectID, QString &ObjectName); //Получить наименование объекта
	PMetaTable* GetMetaTable(const QString &TableName); //Получить указатель на мета-таблицу

protected:
	virtual bool Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	QVariant CheckNullField(const QString &FieldName, ISTcpMessage *TcpMessage); //Проверка наличия поля
	bool ErrorQuery(const QString &LocalError, ISQuery &SqlQuery); //Вернуть ошибку запроса

	virtual bool Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetLastClient(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool UserPasswordExist(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool UserPasswordCreate(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool UserPasswordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool UserPasswordReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool UserSettingsReset(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool UserDeviceAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool UserDeviceDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetRecordCall(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetClients(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool RecordAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool RecordEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool RecordDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool RecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool RecordGetInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool DiscussionAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool DiscussionEdit(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool DiscussionCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetTableData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetTableQuery(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool NoteRecordGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool NoteRecordSet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool FileStorageAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool FileStorageCopy(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool FileStorageGet(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool SearchTaskText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool SearchTaskID(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool SearchFullText(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetCalendarEvents(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool CalendarDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetInternalLists(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool SaveMetaData(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetGroupRights(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GroupRightSubSystemAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GroupRightSubSystemDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GroupRightTableAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GroupRightTableDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GroupRightSpecialAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GroupRightSpecialDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetRecordValue(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool RecordFavoriteAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool RecordFavoriteDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetFavoriteNames(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool FavoritesDelete(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool LogGetStructure(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool LogGetContent(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool CalendarClose(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetHistoryList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool TaskCommentAdd(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetForeignList(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetServerInfo(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool OrganizationFormINN(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);
	virtual bool GetStatement(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer);

protected:
    QString ErrorString; //Текстовое описание ошибки запроса
	QString DBConnectionName;

private:
	QString DBHost;
	int DBPort;
	QString DBName;
	QString DBUser;
	QString DBPassword;
	bool IsStarted; //Флаг успешного запуска воркера
	ISQuery *qProtocol;
	bool IsRunning; //Флаг занятости воркера
	ISTcpMessage *CurrentMessage; //Указатель на текущее сообщение
	bool IsStopped; //Флаг остановки работы воркера

	ISCriticalSection CriticalSection; //Критическая секция для синхронизации
};
//-----------------------------------------------------------------------------
#endif
