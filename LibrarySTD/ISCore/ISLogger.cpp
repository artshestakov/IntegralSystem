#include "ISLogger.h"
#include "ISAlgorithm.h"
#include "ISDebug.h"
#include "ISTypedefs.h"
#include "ISLoggerUDP.h"
//-----------------------------------------------------------------------------
ISLogger::ISLogger()
    : ErrorString(STRING_NO_ERROR),
    LastIndex(0),
    IsRunning(false),
    IsFinished(false),
    CurrentDay(0),
    CurrentMonth(0),
    CurrentYear(0)
{
    CRITICAL_SECTION_INIT(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISLogger::~ISLogger()
{
    CRITICAL_SECTION_DESTROY(&CriticalSection);
}
//-----------------------------------------------------------------------------
ISLogger& ISLogger::Instance()
{
    static ISLogger Logger;
    return Logger;
}
//-----------------------------------------------------------------------------
const std::string& ISLogger::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISLogger::Initialize()
{
    //Получаем текущую дату и запоминаем текущий день
    ISDate CurrentDate = ISDate::CurrentDate();
    CurrentDay = CurrentDate.Day;

    if (!CreateLogDirectory(CurrentDate)) //Ошибка при создании директорий
    {
        return false;
    }

    std::string path_file = GetPathFile(CurrentDate);
    File.open(path_file.c_str(), std::ios::out | std::ios::app);
    if (!File.is_open()) //Не удалось открыть файл
    {
        ErrorString = "not open file \"" + path_file + "\": " + ISAlgorithm::GetLastErrorS();
        return false;
    }

    //Запускаем основной поток логгера
    std::thread(&ISLogger::Worker, this).detach();
    IsRunning = true;
    return IsRunning;
}
//-----------------------------------------------------------------------------
void ISLogger::Shutdown()
{
    if (IsRunning) //Если логгер был запущен - останавливаем
    {
        //Останавливаем логгер
        CRITICAL_SECTION_LOCK(&CriticalSection);
        IsRunning = false;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

        //Ждём когда он остановится и закрываем файл
        while (!IsFinished)
        {
            ISSleep(1);
        }
        File.close();
    }
}
//-----------------------------------------------------------------------------
void ISLogger::Log(ISNamespace::LogMessageType MessageType, const std::string &Component, const char *Format, ...)
{
    if (!IsRunning)
    {
        ISDEBUG_W("Logger is not initialized");
        return;
    }

    //Получаем строковый тип сообщения
    const char *message_type = nullptr;
    switch (MessageType)
    {
    case ISNamespace::LogMessageType::Unknown: message_type = LOG_UNKNOWN; break;
    case ISNamespace::LogMessageType::Debug: message_type = LOG_DEBUG; break;
    case ISNamespace::LogMessageType::Info: message_type = LOG_INFO; break;
    case ISNamespace::LogMessageType::Warning: message_type = LOG_WARNING; break;
    case ISNamespace::LogMessageType::Error: message_type = LOG_ERROR; break;
    case ISNamespace::LogMessageType::Critical: message_type = LOG_CRITICAL; break;
    case ISNamespace::LogMessageType::Trace: message_type = LOG_TRACE; break;
    case ISNamespace::LogMessageType::Assert: message_type = LOG_ASSERT; break;
    }

    //Получаем текущую дату и время
    ISDateTime DT = ISDateTime::CurrentDateTime();

    //Формируем заголовок
    char BufferHeader[LOG_HEADER_SIZE] = { 0 };
    std::sprintf(BufferHeader, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s][%s] ",
        DT.Date.Day, DT.Date.Month, DT.Date.Year % 100,
        DT.Time.Hour, DT.Time.Minute, DT.Time.Second, DT.Time.Milliseconds,
        CURRENT_THREAD_ID(), message_type,
        Component.empty() ? "" : Component.c_str());

    //Результирующая строка
    std::string string_result(BufferHeader);

    //Вытаскиваем аргументы
    va_list Arguments;
    va_start(Arguments, Format);

    //Форматируем строку
    char Buffer[LOG_BUFFER_SIZE] = { 0 };
    int Writed = std::vsnprintf(Buffer, LOG_BUFFER_SIZE, Format, Arguments);
    if ((size_t)Writed > LOG_BUFFER_SIZE) //Переполнение буфера - используем резервный
    {
        int NewSize = ++Writed + LOG_HEADER_SIZE;
        std::vector<char> Vector;
        Vector.resize(NewSize);
        std::vsnprintf(&Vector[0], NewSize, Format, Arguments);
        string_result += &Vector[0];
    }
    else
    {
        string_result += Buffer;
    }
    va_end(Arguments);

    CRITICAL_SECTION_LOCK(&CriticalSection);
#ifdef DEBUG //В отладочной версии выводим строку в консоль
    ISDEBUG_L(string_result);
#ifdef WIN32 //Для Windows выводим строку в консоль Visual Studio
    OutputDebugString((string_result + '\n').c_str());
#endif
#endif
    if (LastIndex == LOG_ARRAY_SIZE) //Если превысили размер массива
    {
        ISDEBUG_E("Log array is full");
    }
    else
    {
        Array[LastIndex++] = string_result;
    }
    CRITICAL_SECTION_UNLOCK(&CriticalSection);
}
//-----------------------------------------------------------------------------
bool ISLogger::CreateLogDirectory(const ISDate &Date)
{
    //Запоминаем текущий месяц и год
    CurrentMonth = Date.Month;
    CurrentYear = Date.Year;

    //Формируем путь к текущей папке
    char Buffer[MAX_PATH];
    sprintf(Buffer, "%s%cLogs%c%d%c%02d%c", ISAlgorithm::GetApplicationDir().c_str(), PATH_SEPARATOR,
        PATH_SEPARATOR, CurrentYear, PATH_SEPARATOR, CurrentMonth, PATH_SEPARATOR);
    PathLogsDir = Buffer;

    if (!ISAlgorithm::DirExist(PathLogsDir)) //Если папка с текущим месяцем не существует - создаём её
    {
        if (!ISAlgorithm::DirCreate(PathLogsDir, ErrorString)) //Ошибка создания папки
        {
            ErrorString = "Error create directory \"" + PathLogsDir + "\": " + ErrorString;
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
std::string ISLogger::GetPathFile(const ISDate &Date) const
{
    char Buffer[MAX_PATH];
    sprintf(Buffer, "%s%s_%02d.%02d.%d.log", PathLogsDir.c_str(), ISAlgorithm::GetApplicationName().c_str(), Date.Day, Date.Month, Date.Year);
    return Buffer;
}
//-----------------------------------------------------------------------------
void ISLogger::Worker()
{
    while (true)
    {
        ISSleep(LOG_TIMEOUT);
        CRITICAL_SECTION_LOCK(&CriticalSection);
        if (LastIndex) //Если в очереди есть сообщения
        {
            for (size_t i = 0; i < LastIndex; ++i)
            {
                std::string LogMessage = Array[i];
                File << LogMessage << std::endl;
                ISLoggerUDP::Instance().Add(LogMessage);
                Array[i].clear(); //Очищаем текущую строку
            }
            LastIndex = 0;
            File.flush();
        }
        bool is_running = IsRunning;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

        if (!is_running)
        {
            break;
        }

        //Если сменился месяц или год - создаём недостающие папки
        ISDate CurrentDate = ISDate::CurrentDate();
        if (CurrentMonth != CurrentDate.Month || CurrentYear != CurrentDate.Year)
        {
            //Пытаемся создать недосающие директории пока не получится
            while (!CreateLogDirectory(CurrentDate))
            {
                ISDEBUG_E(ErrorString);
                ISSleep(LOG_TIMEOUT);
            }
        }

        //Если сменился день - закрываем текущий файл и открываем новый
        if (CurrentDay != CurrentDate.Day)
        {
            File.close();

            bool is_opened = false;
            while (!is_opened) //Пытаемся открыть файл
            {
                std::string path_file = GetPathFile(CurrentDate);
                File.open(path_file, std::ios::out | std::ios::app);
                is_opened = File.is_open();
                if (is_opened) //Файл был успешно открыт - запоминаем текущий день
                {
                    CurrentDay = CurrentDate.Day;
                }
                else //Файл не удалось открыть, пытаемся сделать это ещё раз через секунду
                {
                    ISDEBUG_E("Logger: not open file \"" + path_file + "\". Error: " + ErrorString);
                    ISSleep(1000);
                }
            }
        }
    }
    IsFinished = true;
}
//-----------------------------------------------------------------------------
