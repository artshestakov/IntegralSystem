#include "StdAfx.h"
#include "ISCrashDumper.h"
#include "EXDefines.h"
#include "ISDebug.h"
#include "ISCore.h"
#include "ISSystem.h"
//-----------------------------------------------------------------------------
ISCrashDumper::ISCrashDumper() : QObject()
{

}
//-----------------------------------------------------------------------------
ISCrashDumper::~ISCrashDumper()
{

}
//-----------------------------------------------------------------------------
void ISCrashDumper::Startup()
{
	if (!ISSystem::IsConfigurationDebug())
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

		ISDebug::ShowInfoString(QString("Install CrashDumper for: %1").arg(ISSystem::GetCurrentOSName()));
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef WIN32
#include "ISStackWalker.h"
int ISCrashDumper::ReportHook(int ReportType, char *Message, int *ReturnValue)
{
    CreateReport(LOG_FILE_CRASH, Message, nullptr);
	return 0;
}
LONG ISCrashDumper::ExceptionHandling(_EXCEPTION_POINTERS *ExceptionInfo)
{
    CreateReport(LOG_FILE_ASSERT, "Exception handling", ExceptionInfo);
	return EXCEPTION_EXECUTE_HANDLER;
}
void ISCrashDumper::CreateReport(const QString &FileName, char *Text, _EXCEPTION_POINTERS *ExceptionInfo)
{
    ISStackWalker stack_walker;

	if (ExceptionInfo)
	{
        stack_walker.ShowCallstack(GetCurrentThread(), ExceptionInfo->ContextRecord);
	}
	else
	{
        stack_walker.ShowCallstack(GetCurrentThread());
	}

    QFile FileCrash(GetLogPath(FileName));
	if (FileCrash.open(QIODevice::WriteOnly))
	{
        FileCrash.write("=========================================DEBUG=======================================\r\n");

		for (int i = 0; i < ISDebug::GetInstance().GetDebugMessages().count(); i++)
		{
			FileCrash.write(ISDebug::GetInstance().GetDebugMessages().at(i).toUtf8() + "\r\n");
		}

        FileCrash.write("=========================================STACK=======================================\r\n");
		FileCrash.write(stack_walker.GetCallStack().toUtf8());
		FileCrash.close();
	}

	QString CrashMessage;
	if (ISDebug::GetInstance().GetAssertMessage().length())
	{
		CrashMessage = ISDebug::GetInstance().GetAssertMessage();
	}
	else
	{
		CrashMessage = QString::fromLocal8Bit(Text);
	}

	if (ISSystem::GetApplicationType() == ISNamespace::AT_GUI)
	{
		QErrorMessage *ErrorMessage = new QErrorMessage(nullptr);
		ErrorMessage->showMessage(CrashMessage);
		ErrorMessage->exec();
	}

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

        for (int i = 0; i < ISDebug::GetInstance().GetDebugMessages().count(); i++)
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
    return APPLICATION_LOGS_PATH + "/" + FileName + "-" + qAppName() + "_" + QDateTime::currentDateTime().toString(DATE_TIME_FORMAT_V8) + ".txt";
}
//-----------------------------------------------------------------------------
