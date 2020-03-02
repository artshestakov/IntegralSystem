#include "ISCrashDumper.h"
#include "ISConstants.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
void ISCrashDumper::Init()
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
    CreateReport(NULL, Message ? std::string(Message) : std::string());
	return 0;
}
LONG ISCrashDumper::ExceptionHandling(_EXCEPTION_POINTERS *ExceptionInfo)
{
	CreateReport(ExceptionInfo, "ExceptionHandling()");
	return EXCEPTION_EXECUTE_HANDLER;
}
void ISCrashDumper::CreateReport(_EXCEPTION_POINTERS *ExceptionInfo, const std::string &Message)
{
	if (!Message.empty())
	{
		printf("Crash: %s", Message.c_str());
	}

    ISStackWalker stack_walker;
	stack_walker.ShowCallstack(GetCurrentThread(), ExceptionInfo ? ExceptionInfo->ContextRecord : NULL);

	std::string FilePath = QString(ISDefines::Core::PATH_CRASH_DIR + "/" + QDateTime::currentDateTime().toString(DATE_TIME_FORMAT_V8) + "." + EXTENSION_CRASH).toStdString();
	FILE *File = fopen(FilePath.c_str(), "w");
	if (File)
	{
		std::string Content = stack_walker.GetCallStack();
		if (!Message.empty())
		{
			Content.insert(0, "\n");
			Content.insert(0, "====================================================================");
			Content.insert(0, Message);
		}

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

	if (ISDefines::Core::IS_GUI)
	{
		QProcess::startDetached(ISDefines::Core::PATH_APPLICATION_DIR + "/ErrorViewer.exe", QStringList() << QString::fromStdString(FilePath));
	}
	else
	{
		printf("Crash\n");
	}
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

    ISLOGGER_ERROR(QString("%1 Received signal: %2").arg(FileName).arg(Signum));

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

    //ISCore::ExitApplication(); //???
}
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
