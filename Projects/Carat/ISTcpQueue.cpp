#include "ISTcpQueue.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISLogger.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QS_MESSAGE_ID = PREPARE_QUERY("SELECT sgdb_tcpmessageid "
											 "FROM _settingsdatabase "
											 "WHERE sgdb_uid = :SettingUID");
//-----------------------------------------------------------------------------
static QString QU_MESSAGE_ID = PREPARE_QUERY("UPDATE _settingsdatabase SET "
											 "sgdb_tcpmessageid = :MessageID "
											 "WHERE sgdb_uid = :SettingUID");
//-----------------------------------------------------------------------------
ISTcpQueue::ISTcpQueue()
	: MessageID(0),
	IsActive(true)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpQueue::~ISTcpQueue()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpQueue& ISTcpQueue::Instance()
{
	static ISTcpQueue TcpQueue;
	return TcpQueue;
}
//-----------------------------------------------------------------------------
void ISTcpQueue::ReadMessageID()
{
	ISQuery qSelectMessageID(QS_MESSAGE_ID);
	qSelectMessageID.BindValue(":SettingUID", CONST_UID_SETTINGS_DATABASE);
	if (qSelectMessageID.Execute())
	{
		if (qSelectMessageID.First())
		{
			bool Ok = false;
			MessageID = qSelectMessageID.ReadColumn("sgdb_tcpmessageid").toUInt(&Ok);
			if (!Ok)
			{
				ISLOGGER_E(__CLASS__, "Invalid reading message id");
			}
		}
		else
		{
			ISLOGGER_E(__CLASS__, "Not exist setting database");
		}
	}
	else
	{
		ISLOGGER_E(__CLASS__, "Not getting message id from database");
	}
}
//-----------------------------------------------------------------------------
void ISTcpQueue::WriteMessageID()
{
	ISQuery qUpdateMessageID(QU_MESSAGE_ID);
	qUpdateMessageID.BindValue(":MessageID", MessageID);
	qUpdateMessageID.BindValue(":SettingUID", CONST_UID_SETTINGS_DATABASE);
	if (!qUpdateMessageID.Execute())
	{
		ISLOGGER_E(__CLASS__, "Not save message id: " + qUpdateMessageID.GetErrorString());
	}
}
//-----------------------------------------------------------------------------
void ISTcpQueue::Shutdown()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsActive = false;
	while (!Queue.empty())
	{
		ISTcpMessage *TcpMessage = Queue.front();
		Queue.pop();
		delete TcpMessage;
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpQueue::AddMessage(ISTcpMessage *TcpMessage)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	if (IsActive) //Если очередь активна - добавляем сообщение в неё
	{
		TcpMessage->MessageID = ++MessageID;
		Queue.push(TcpMessage);
	}
	else //Иначе удаляем
	{
		delete TcpMessage;
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpMessage* ISTcpQueue::GetMessage()
{
	ISTcpMessage *TcpMessage = nullptr;
	//Блокируем критическую секцию, забираем очередное сообщение и разблокируем секцию
	CRITICAL_SECTION_LOCK(&CriticalSection);
	if (IsActive) //Если очередь активна - возвращем очередное сообщение
	{
		if (!Queue.empty())
		{
			TcpMessage = Queue.front();
			Queue.pop();
		}
	}
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return TcpMessage;
}
//-----------------------------------------------------------------------------
