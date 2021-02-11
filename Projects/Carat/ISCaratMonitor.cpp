#include "ISCaratMonitor.h"
#include "ISAlgorithm.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISQuery.h"
//-----------------------------------------------------------------------------
static QString QI_MONITOR = PREPARE_QUERY("INSERT INTO _monitor(mntr_memory) "
										  "VALUES(:Memory)");
//-----------------------------------------------------------------------------
ISCaratMonitor::ISCaratMonitor()
	: ErrorString(NO_ERROR_STRING),
	IsRunning(false),
	IsFinished(false)
{
	CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISCaratMonitor::~ISCaratMonitor()
{
	CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISCaratMonitor& ISCaratMonitor::Instance()
{
	static ISCaratMonitor CaratMonitor;
	return CaratMonitor;
}
//-----------------------------------------------------------------------------
QString ISCaratMonitor::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISCaratMonitor::Start()
{
	//��������� ����� � ��� �������
	std::thread(&ISCaratMonitor::Process, this).detach();
	ISSleep(1000);
	
	CRITICAL_SECTION_LOCK(&CriticalSection);
	bool is_running = IsRunning;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
	return is_running;
}
//-----------------------------------------------------------------------------
void ISCaratMonitor::Shutdown()
{
	if (!IsRunning)
	{
		return;
	}

	//������������� ���� ��������� ������
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = false;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);

	//��� ���� �� �� �����������
	while (true)
	{
		ISSleep(1);
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_finished = IsFinished;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (is_finished)
		{
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void ISCaratMonitor::Process()
{
	//������������ � ����
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = ISDatabase::Instance().Connect(CONNECTION_MONITOR, ISDatabase::Instance().GetOption(CONNECTION_DEFAULT));
	CRITICAL_SECTION_UNLOCK(&CriticalSection);

	if (!IsRunning) //�� ������� ������������ � ����
	{
		ErrorString = ISDatabase::Instance().GetErrorString();
		return;
	}

	//����������� � ���� ������ ������� - �������� ������ ������
	while (true)
	{
		ISSleep(5000);
		{
			//�������� ���������� � ��������� � ����
			ISQuery qInsert(ISDatabase::Instance().GetDB(CONNECTION_MONITOR), QI_MONITOR);
			qInsert.BindValue(":Memory", GetMemory());
			if (!qInsert.Execute()) //������ �������
			{
				ISLOGGER_E(__CLASS__, "Not insert monitor indicators: " + qInsert.GetErrorString());
			}
		}

		//���������, �� ���������� �� �����
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_running = IsRunning;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (!is_running) //������ ���������� - ����������� �� �� � �������
		{
			ISDatabase::Instance().Disconnect(CONNECTION_MONITOR);
			break;
		}
	}
	IsFinished = true;
}
//-----------------------------------------------------------------------------
quint64 ISCaratMonitor::GetMemory() const
{
	unsigned long Result = 0;
#ifdef WIN32
	PROCESS_MEMORY_COUNTERS ProcessMemory = { 0 };
	if (GetProcessMemoryInfo(GetCurrentProcess(), &ProcessMemory, sizeof(PROCESS_MEMORY_COUNTERS)) == TRUE)
	{
		Result = ProcessMemory.WorkingSetSize / 1024;
	}
	else
	{
		ISLOGGER_E(__CLASS__, "Not getting memory: " + ISAlgorithm::GetLastErrorString());
	}
#else
#endif
	return Result;
}
//-----------------------------------------------------------------------------
