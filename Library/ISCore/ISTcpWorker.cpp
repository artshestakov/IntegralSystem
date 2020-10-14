#include "ISTcpWorker.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpWorker::ISTcpWorker()
	: QObject(),
	ErrorString(NO_ERROR_STRING),
	IsRunning(false),
	CurrentMessage(nullptr)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpWorker::~ISTcpWorker()
{
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetRunning()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	bool is_running = IsRunning;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return is_running;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::SetMessage(ISTcpMessage *TcpMessage)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = true;
	CurrentMessage = TcpMessage;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Run()
{
	while (true)
	{
		//Засыпаем на одну милисекунду и даём поработать потоку
		ISSleep(1);

		//Получаем текущее сообщение
		CRITICAL_SECTION_LOCK(&CriticalSection);
		ISTcpMessage *tcp_message = CurrentMessage;
		CurrentMessage = nullptr;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);

		//Если появилось сообщение на обработку - выполняем его
		if (tcp_message)
		{
			bool Result = false;
			switch (tcp_message->Type)
			{
			case ISNamespace::AMT_Auth:
				Result = Auth(tcp_message);
				break;
			}
			Finish();
		}
	}
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Finish()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = false;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CheckNullField(const QString &FieldName, const QVariantMap &VariantMap)
{
	bool Result = VariantMap.contains(FieldName);
	if (!Result)
	{
		ErrorString = "field \"" + FieldName + "\" not exist";
	}
	return Result;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Auth(ISTcpMessage *TcpMessage)
{
	return true;
}
//-----------------------------------------------------------------------------
