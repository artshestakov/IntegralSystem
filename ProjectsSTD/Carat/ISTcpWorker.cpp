#include "ISTcpWorker.h"
#include "ISAlgorithm.h"
#include "ISConstants.h"
#include "ISTcpQueue.h"
//-----------------------------------------------------------------------------
ISTcpWorker::ISTcpWorker()
	: ErrorString(STRING_NO_ERROR),
	IsBusy(false),
	IsRunning(false),
	IsFinished(false),
	CurrentMessage(nullptr)
{
	MapFunction[API_AUTH] = &ISTcpWorker::Auth;
	MapFunction[API_SLEEP] = &ISTcpWorker::Sleep;

	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISTcpWorker::~ISTcpWorker()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::GetBusy()
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	bool is_busy = IsBusy;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return is_busy;
}
//-----------------------------------------------------------------------------
void ISTcpWorker::SetMessage(ISTcpMessage *TcpMessage)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	CurrentMessage = TcpMessage;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Start()
{
	std::thread(&ISTcpWorker::Process, this).detach();
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Shutdown()
{
	//������������� ������
	CRITICAL_SECTION_INIT(&CriticalSection);
	IsRunning = false;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);

	//��� ���� ������ �� �����������
	while (true)
	{
		ISSleep(1);

		CRITICAL_SECTION_INIT(&CriticalSection);
		bool is_finished = IsFinished;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (is_finished) //������ ����������� - ������� �� �����
		{
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISTcpWorker::Process()
{
	IsRunning = true;
	while (true)
	{
		ISSleep(1); //������� ������

		//���������, �� ������������� �� ������
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_running = IsRunning;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (!is_running) //������ ���������� - ������� �� ������
		{
			break;
		}

		//�������� ������� ���������
		CRITICAL_SECTION_LOCK(&CriticalSection);
		ISTcpMessage *tcp_message = CurrentMessage;
		if (tcp_message) 
		{
			IsBusy = true;
			CurrentMessage = nullptr;
		}
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		
		//���� ��������� ��� - ��������� �� ��������� ��������
		if (!tcp_message)
		{
			continue;
		}

		bool Result = false;
		unsigned long long PerfomanceMsec = 0;
		ISTcpAnswer *TcpAnswer = new ISTcpAnswer(tcp_message->Socket);

		if (tcp_message->IsValid()) //���� ��������� �������� - ��������� � ����������
		{
			//���� ������ �� ��������������� � ������ ��� �� ������������� - ������
			//if (tcp_message->Type != API_AUTH && !tcp_message->TcpSocket->GetAuthorized())
			{
				//ErrorString = LANG("Carat.Error.NotAuthorized");
			}
			//else //������ ������������� - ����������
			{
				ISTimePoint TimePoint = ISAlgorithm::GetTick(); //���������� ������� �����
				Result = Execute(tcp_message, TcpAnswer);
				PerfomanceMsec = ISAlgorithm::GetTickDiff(ISAlgorithm::GetTick(), TimePoint);
			}
		}
		else //��������� �� ��������
		{
			//ErrorString = tcp_message->GetErrorString();
		}
		delete tcp_message;

		if (!Result)
		{
			TcpAnswer->SetError(ErrorString);
		}
		ISTcpQueue::Instance().AddAnswer(TcpAnswer);

		CRITICAL_SECTION_LOCK(&CriticalSection);
		IsBusy = false;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
	}

	//��������� ���� ���������
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsFinished = true;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Execute(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	//���� ��������� �� ������� � ���� ����� - ���������
	auto It = MapFunction.find(TcpMessage->Type);
	if (It != MapFunction.end())
	{
		return (this->*It->second)(TcpMessage, TcpAnswer);
	}
	else //������� �� �������
	{
		//ErrorString = LANG("Carat.Error.NotExistFunction").arg(TcpMessage->Type);
	}
	return false;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::CheckIsNull(ISTcpMessage *TcpMessage, const std::string &ParameterName)
{
	if (!TcpMessage->Parameters.HasMember(ParameterName.c_str()))
	{
		ErrorString = "parameters is empty";
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Auth(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	IS_UNUSED(TcpMessage);
	IS_UNUSED(TcpAnswer);
	return true;
}
//-----------------------------------------------------------------------------
bool ISTcpWorker::Sleep(ISTcpMessage *TcpMessage, ISTcpAnswer *TcpAnswer)
{
	IS_UNUSED(TcpAnswer);

	if (!CheckIsNull(TcpMessage, "Timeout"))
	{
		return false;
	}

	rapidjson::Value &ValueTimeout = TcpMessage->Parameters["Timeout"];
	if (!ValueTimeout.IsInt()) //�������� �� �������� ��������
	{
		ErrorString = "The value is not a integer";
		return false;
	}

	int Timeout = ValueTimeout.GetInt();
	if (Timeout < 0) //�������� ������ ����
	{
		ErrorString = "The value cannot be less than zero";
		return false;
	}

	if (Timeout == 0) //�������� ����� ����
	{
		ErrorString = "The value cannot be zero";
		return false;
	}
	ISSleep(Timeout);
	return true;
}
//-----------------------------------------------------------------------------
