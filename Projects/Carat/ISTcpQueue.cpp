#include "ISTcpQueue.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISTcpQueue::ISTcpQueue()
	: FilePath(QCoreApplication::applicationDirPath() + "/Temp/MessageID"),
	File(FilePath),
	MessageID(0),
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
	if (File.exists()) //Если файл MessageID существует - читаем его
	{
		if (File.open(QIODevice::ReadOnly)) //Файл успешно открылся
		{
			//Читаем и закрываем
			QString Content = File.readAll();
			File.close();

			bool Ok = true;
			MessageID = Content.toUInt(&Ok);
			if (!Ok) //Не удалось привести данные файла к числу
			{
				ISLOGGER_W(__CLASS__, QString("Invalid content with MessageID file (%1): %2").arg(FilePath).arg(Content));
			}
		}
		else //Файл не открылся
		{
			ISLOGGER_W(__CLASS__, "Not open file: " + File.errorString());
		}
	}
}
//-----------------------------------------------------------------------------
void ISTcpQueue::WriteMessageID()
{
	if (File.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		std::stringstream StringStream;
		StringStream << MessageID;
		std::string String = StringStream.str();
		if (File.write(StringStream.str().c_str()) == (qint64)String.size())
		{
			File.close();
		}
		else
		{
			ISLOGGER_W(__CLASS__, "Not write MessageID: " + File.errorString());
		}
	}
	else
	{
		ISLOGGER_W(__CLASS__, "Not open file: " + File.fileName());
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
