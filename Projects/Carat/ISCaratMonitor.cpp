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
	//Запускаем поток и ждём секунду
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

	//Устанавливаем флаг остановки потока
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = false;
	CRITICAL_SECTION_UNLOCK(&CriticalSection);

	//Ждём пока он не остановится
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
	//Подключаемся к базе
	CRITICAL_SECTION_LOCK(&CriticalSection);
	IsRunning = ISDatabase::Instance().Connect(CONNECTION_MONITOR, ISDatabase::Instance().GetOption(CONNECTION_DEFAULT));
	CRITICAL_SECTION_UNLOCK(&CriticalSection);

	if (!IsRunning) //Не удалось подключиться к базе
	{
		ErrorString = ISDatabase::Instance().GetErrorString();
		return;
	}

	//Подключение к базе прошло успешно - начинаем работу потока
	while (true)
	{
		ISSleep(5000);
		{
			//Получаем показатели и добавляем в базу
			ISQuery qInsert(ISDatabase::Instance().GetDB(CONNECTION_MONITOR), QI_MONITOR);
			qInsert.BindValue(":Memory", GetMemory());
			if (!qInsert.Execute()) //Ошибка вставки
			{
				ISLOGGER_E(__CLASS__, "Not insert monitor indicators: " + qInsert.GetErrorString());
			}
		}

		//Проверяем, не остановлен ли поток
		CRITICAL_SECTION_LOCK(&CriticalSection);
		bool is_running = IsRunning;
		CRITICAL_SECTION_UNLOCK(&CriticalSection);
		if (!is_running) //Потока остановлен - отключаемся от БД и выходим
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
    else //Не удалось получить информацию о памяти процесса
	{
		ISLOGGER_E(__CLASS__, "Not getting memory: " + ISAlgorithm::GetLastErrorString());
	}
#else
    QFile File(QString("/proc/%1/status").arg(CURRENT_PID()));
    if (File.open(QIODevice::ReadOnly)) //Файл открылся - читаем его и закрываем
    {
        QString Content = File.readAll();
        File.close();

        //Пробегаемся по всем строкам и ищем нужное поле
        QStringList StringList = Content.split('\n');
        for (const QString &String : StringList)
        {
            if (String.toLower().contains("rssanon")) //Нашли - вытаскиваем из строки только цифры
            {
                QString Temp;
                for (const QChar &Char : String)
                {
                    if (Char.isDigit()) //Текущий символ - цифра
                    {
                        Temp.push_back(Char);
                    }
                }

                bool Ok = true;
                Result = Temp.toULong(&Ok);
                if (!Ok) //Не удалось преобразовать строку в число
                {
                    ISLOGGER_E(__CLASS__, QString("Not convert '%1' to unsigned long").arg(Temp));
                }
                break; //Выходим из цикла
            }
        }
    }
    else //Не удалось открыть файл
    {
        ISLOGGER_E(__CLASS__, QString("Not error proc-file (%1): %2").arg(File.fileName()).arg(File.errorString()));
    }
#endif
	return Result;
}
//-----------------------------------------------------------------------------
