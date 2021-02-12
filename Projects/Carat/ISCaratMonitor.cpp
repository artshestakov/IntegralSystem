#include "ISCaratMonitor.h"
#include "ISAlgorithm.h"
#include "ISDatabase.h"
#include "ISLogger.h"
#include "ISQuery.h"
#include "ISTcpClients.h"
#include "ISConfig.h"
//-----------------------------------------------------------------------------
static QString QI_MONITOR = PREPARE_QUERY("INSERT INTO _monitor(mntr_memory, mntr_clients, mntr_tcpquerytimeavg) "
										  "VALUES(:Memory, :Clients, :TCPQueryTimeAvg)");
//-----------------------------------------------------------------------------
ISCaratMonitor::ISCaratMonitor()
	: ErrorString(NO_ERROR_STRING),
	IsRunning(false),
	IsFinished(false),
	Timeout(0),
	TCPQueryTime(0),
	TCPQueryCount(0)
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
void ISCaratMonitor::RegisterQueryTime(unsigned long long MSec)
{
	CRITICAL_SECTION_LOCK(&CriticalSection);
	TCPQueryTime += MSec;
	++TCPQueryCount;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
QString ISCaratMonitor::GetErrorString() const
{
	return ErrorString;
}
//-----------------------------------------------------------------------------
void ISCaratMonitor::Start()
{
	//�������� �������� �������� �� ����������������� �����
	Timeout = CONFIG_INT(CONST_CONFIG_MONITOR_TIMEOUT);

	//��������� ����� � ��� �������
	std::thread(&ISCaratMonitor::Process, this).detach();
	ISLOGGER_I(__CLASS__, QString("starting with timeout %1 seconds").arg(Timeout));
	Timeout *= 1000;
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
	IsRunning = ISDatabase::Instance().Connect(CONNECTION_MONITOR, ISDatabase::Instance().GetOption(CONNECTION_DEFAULT));
	if (!IsRunning) //�� ������� ������������ � ����
	{
		ISLOGGER_E(__CLASS__, "Not connected to database: " + ISDatabase::Instance().GetErrorString());
		return;
	}

	//������ ��������� �� ������ ������� ���, ����� ����� ������������ ��� ���������
	ISQuery *qInsert = new ISQuery(ISDatabase::Instance().GetDB(CONNECTION_MONITOR), QI_MONITOR);

	//����������� � ���� ������ ������� - �������� ������ ������
	while (true)
	{
		ISSleep(Timeout);
		
		//��������� ���������� � ����		
		qInsert->BindValue(":Memory", GetMemory());
		qInsert->BindValue(":Clients", ISTcpClients::Instance().GetCount());
		qInsert->BindValue(":TCPQueryTimeAvg", GetTCPTimeAvg());
		if (!qInsert->Execute()) //������ �������
		{
			ISLOGGER_E(__CLASS__, "Not insert monitor indicators: " + qInsert->GetErrorString());
		}
		
		//���������, �� ���������� �� �����
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_running = IsRunning;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (!is_running) //������ ���������� - ����������� �� �� � �������
		{
			delete qInsert;
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
    else //�� ������� �������� ���������� � ������ ��������
	{
		ISLOGGER_E(__CLASS__, "Not getting memory: " + ISAlgorithm::GetLastErrorString());
	}
#else
    QFile File(QString("/proc/%1/status").arg(CURRENT_PID()));
    if (File.open(QIODevice::ReadOnly)) //���� �������� - ������ ��� � ���������
    {
        QString Content = File.readAll();
        File.close();

        //����������� �� ���� ������� � ���� ������ ����
        QStringList StringList = Content.split('\n');
        for (const QString &String : StringList)
        {
            if (String.toLower().contains("rssanon")) //����� - ����������� �� ������ ������ �����
            {
                QString Temp;
                for (const QChar &Char : String)
                {
                    if (Char.isDigit()) //������� ������ - �����
                    {
                        Temp.push_back(Char);
                    }
                }

                bool Ok = true;
                Result = Temp.toULong(&Ok);
                if (!Ok) //�� ������� ������������� ������ � �����
                {
                    ISLOGGER_E(__CLASS__, QString("Not convert '%1' to unsigned long").arg(Temp));
                }
                break; //������� �� �����
            }
        }
    }
    else //�� ������� ������� ����
    {
        ISLOGGER_E(__CLASS__, QString("Not error proc-file (%1): %2").arg(File.fileName()).arg(File.errorString()));
    }
#endif
	return Result;
}
//-----------------------------------------------------------------------------
unsigned long long ISCaratMonitor::GetTCPTimeAvg() const
{
	if (TCPQueryTime > 0 && TCPQueryCount > 0)
	{
		return TCPQueryTime / TCPQueryCount;
	}
	return 0;
}
//-----------------------------------------------------------------------------
