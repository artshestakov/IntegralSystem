#include "ISCrashDumper.h"
#include "EXDefines.h"
#include "ISDebug.h"
#include "ISCore.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
void ISCrashDumper::Startup()
{
#ifdef WIN32
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ISCrashDumper::ExceptionHandling);
	_CrtSetReportHook(&ISCrashDumper::ReportHook);
#else
	signal(SIGTERM, OnSystemSignal);
	signal(SIGSEGV, OnSystemSignal);
	signal(SIGABRT, OnSystemSignal);
	signal(SIGINT, OnSystemSignal);
#endif
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef WIN32
#include "ISStackWalker.h"
int ISCrashDumper::ReportHook(int ReportType, char *Message, int *ReturnValue)
{
    CreateReport(Message, nullptr);
	return 0;
}
LONG ISCrashDumper::ExceptionHandling(_EXCEPTION_POINTERS *ExceptionInfo)
{
    CreateReport("Exception handling", ExceptionInfo);
	return EXCEPTION_EXECUTE_HANDLER;
}
void ISCrashDumper::CreateReport(char *Text, _EXCEPTION_POINTERS *ExceptionInfo)
{
    ISStackWalker stack_walker;
	stack_walker.ShowCallstack(GetCurrentThread(), ExceptionInfo ? ExceptionInfo->ContextRecord : NULL);

	std::string FilePath = QString(PATH_LOGS_DIR + "/Crash_" + QDateTime::currentDateTime().toString(DATE_TIME_FORMAT_V8) + "." + EXTENSION_LOG).toStdString();
	FILE *File = fopen(FilePath.c_str(), "w");
	if (File)
	{
		std::string Content = stack_walker.GetCallStack();
		if (fwrite(Content.c_str(), sizeof(char), Content.size(), File) == Content.size())
		{
			printf("Write crash file (%s) - done\n", FilePath.c_str());
		}
		else
		{
			printf("Write crash file (%s) - error\n", FilePath.c_str());
		}
		fclose(File);
	}
	else
	{
		printf("Error open crash file (%s): %s\n", FilePath.c_str(), strerror(errno));
	}

#ifdef GUI
	/*QErrorMessage *ErrorMessage = new QErrorMessage();
	ErrorMessage->showMessage(Text, "MessageType");
	ErrorMessage->exec();*/
	MessageBox(NULL, "1", "2", MB_OK | MB_ICONERROR | MB_DEFBUTTON1 | MB_TOPMOST);
#else
	printf("123: %s\n", Text);
#endif

	ISCore::ExitApplication();
}
//-----------------------------------------------------------------------------
#else
#include "call_stack.hpp"
void ISCrashDumper::OnSystemSignal(int Signum)
{
    stacktrace::call_stack stack_trace;

    QString FileName;
	if (Signum == SIGINT)
	{
        FileName = LOG_FILE_ASSERT;
	}
    else
    {
        FileName = LOG_FILE_CRASH;
    }

    ISDebug::ShowCriticalString(QString("%1 Received signal: %2").arg(FileName).arg(Signum));

    QFile FileCrash(GetLogPath(FileName));
    if (FileCrash.open(QIODevice::WriteOnly))
    {
        FileCrash.write("=========================================DEBUG=======================================\r\n");

        for (int i = 0; i < ISDebug::GetInstance().GetDebugMessages().count(); ++i)
        {
            FileCrash.write(ISDebug::GetInstance().GetDebugMessages().at(i).toUtf8() + "\r\n");
        }

        FileCrash.write("=========================================STACK=======================================\r\n");
        FileCrash.write(QString::fromStdString(stack_trace.to_string()).toUtf8());
        FileCrash.close();
    }

    ISCore::ExitApplication();
}
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
QString ISCrashDumper::GetLogPath(const QString &FileName)
{
    return PATH_LOGS_DIR + '/' + FileName + SYMBOL_MINUS + qAppName() + '_' + QDateTime::currentDateTime().toString(DATE_TIME_FORMAT_V8) + ".txt";
}
//-----------------------------------------------------------------------------
