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
    //�������� ������� ���� � ���������� ������� ����
    ISDate CurrentDate = ISDate::CurrentDate();
    CurrentDay = CurrentDate.Day;

    if (!CreateLogDirectory(CurrentDate)) //������ ��� �������� ����������
    {
        return false;
    }

    std::string path_file = GetPathFile(CurrentDate);
    File.open(path_file.c_str(), std::ios::out | std::ios::app);
    if (!File.is_open()) //�� ������� ������� ����
    {
        ErrorString = "not open file \"" + path_file + "\": " + ISAlgorithm::GetLastErrorS();
        return false;
    }

    //��������� �������� ����� �������
    std::thread(&ISLogger::Worker, this).detach();
    IsRunning = true;
    return IsRunning;
}
//-----------------------------------------------------------------------------
void ISLogger::Shutdown()
{
    if (IsRunning) //���� ������ ��� ������� - �������������
    {
        //������������� ������
        CRITICAL_SECTION_LOCK(&CriticalSection);
        IsRunning = false;
        CRITICAL_SECTION_UNLOCK(&CriticalSection);

        //��� ����� �� ����������� � ��������� ����
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

    //�������� ��������� ��� ���������
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

    //�������� ������� ���� � �����
    ISDateTime DT = ISDateTime::CurrentDateTime();

    //��������� ���������
    char BufferHeader[LOG_HEADER_SIZE] = { 0 };
    std::sprintf(BufferHeader, "%02d.%02d.%02d %02d:%02d:%02d:%03d\t%lu\t[%s][%s] ",
        DT.Date.Day, DT.Date.Month, DT.Date.Year % 100,
        DT.Time.Hour, DT.Time.Minute, DT.Time.Second, DT.Time.Milliseconds,
        CURRENT_THREAD_ID(), message_type,
        Component.empty() ? "" : Component.c_str());

    //�������������� ������
    std::string string_result(BufferHeader);

    //����������� ���������
    va_list Arguments;
    va_start(Arguments, Format);

    //����������� ������
    char Buffer[LOG_BUFFER_SIZE] = { 0 };
    int Writed = std::vsnprintf(Buffer, LOG_BUFFER_SIZE, Format, Arguments);
    if ((size_t)Writed > LOG_BUFFER_SIZE) //������������ ������ - ���������� ���������
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
#ifdef DEBUG //� ���������� ������ ������� ������ � �������
    ISDEBUG_L(string_result);
#ifdef WIN32 //��� Windows ������� ������ � ������� Visual Studio
    OutputDebugString((string_result + '\n').c_str());
#endif
#endif
    if (LastIndex == LOG_ARRAY_SIZE) //���� ��������� ������ �������
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
    //���������� ������� ����� � ���
    CurrentMonth = Date.Month;
    CurrentYear = Date.Year;

    //��������� ���� � ������� �����
    char Buffer[MAX_PATH];
    sprintf(Buffer, "%s%cLogs%c%d%c%02d%c", ISAlgorithm::GetApplicationDir().c_str(), PATH_SEPARATOR,
        PATH_SEPARATOR, CurrentYear, PATH_SEPARATOR, CurrentMonth, PATH_SEPARATOR);
    PathLogsDir = Buffer;

    if (!ISAlgorithm::DirExist(PathLogsDir)) //���� ����� � ������� ������� �� ���������� - ������ �
    {
        if (!ISAlgorithm::DirCreate(PathLogsDir, ErrorString)) //������ �������� �����
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
        if (LastIndex) //���� � ������� ���� ���������
        {
            for (size_t i = 0; i < LastIndex; ++i)
            {
                std::string LogMessage = Array[i];
                File << LogMessage << std::endl;
                ISLoggerUDP::Instance().Add(LogMessage);
                Array[i].clear(); //������� ������� ������
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

        //���� �������� ����� ��� ��� - ������ ����������� �����
        ISDate CurrentDate = ISDate::CurrentDate();
        if (CurrentMonth != CurrentDate.Month || CurrentYear != CurrentDate.Year)
        {
            //�������� ������� ���������� ���������� ���� �� ���������
            while (!CreateLogDirectory(CurrentDate))
            {
                ISDEBUG_E(ErrorString);
                ISSleep(LOG_TIMEOUT);
            }
        }

        //���� �������� ���� - ��������� ������� ���� � ��������� �����
        if (CurrentDay != CurrentDate.Day)
        {
            File.close();

            bool is_opened = false;
            while (!is_opened) //�������� ������� ����
            {
                std::string path_file = GetPathFile(CurrentDate);
                File.open(path_file, std::ios::out | std::ios::app);
                is_opened = File.is_open();
                if (is_opened) //���� ��� ������� ������ - ���������� ������� ����
                {
                    CurrentDay = CurrentDate.Day;
                }
                else //���� �� ������� �������, �������� ������� ��� ��� ��� ����� �������
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
