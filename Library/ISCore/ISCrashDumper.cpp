#include "ISCrashDumper.h"
#include "ISConstants.h"
#include "ISDefinesCore.h"
//-----------------------------------------------------------------------------
char *AssertMessage = NULL;
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
void ISCrashDumper::SetAssertMessage(const char *assert_message)
{
    AssertMessage = (char *)malloc(strlen(assert_message) + 1);
    if (AssertMessage)
    {
        strcpy(AssertMessage, assert_message);
    }
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
    ISStackWalker stack_walker;
    stack_walker.ShowCallstack(GetCurrentThread(), ExceptionInfo ? ExceptionInfo->ContextRecord : NULL);

    std::string FilePath = QString(ISDefines::Core::PATH_CRASH_DIR + "/" + ISDefines::Core::APPLICATION_NAME + '_' + QDateTime::currentDateTime().toString(FORMAT_DATE_TIME_V8) + "." + EXTENSION_CRASH).toStdString();
    FILE *File = fopen(FilePath.c_str(), "w");
    if (File)
    {
        std::string Content = stack_walker.GetCallStack();

        if (!Message.empty())
        {
            Content.insert(0, "\n================================\n");
            Content.insert(0, Message);
        }

        if (AssertMessage)
        {
            Content.insert(0, "\n================================\n");
            Content.insert(0, AssertMessage);
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
		MessageBox(NULL, Message.empty() ? "Unknown reason" : Message.c_str(), "Crash", MB_OK | MB_ICONERROR | MB_DEFBUTTON1 | MB_SYSTEMMODAL);
    }
    else
    {
        printf("Crash: %s\n", Message.empty() ? "Unknown reason" : Message.c_str());
        if (AssertMessage)
        {
            printf("%s", AssertMessage);
        }
    }
}
//-----------------------------------------------------------------------------
#else
#include "call_stack.hpp"
void ISCrashDumper::OnSystemSignal(int SigNum)
{
    stacktrace::call_stack stack_trace;

    std::string FilePath = QString(ISDefines::Core::PATH_CRASH_DIR + "/" + ISDefines::Core::APPLICATION_NAME + '_' + QDateTime::currentDateTime().toString(FORMAT_DATE_TIME_V8) + "." + EXTENSION_CRASH).toStdString();
    FILE *File = fopen(FilePath.c_str(), "w");
    if (File)
    {
        std::string Content = stack_trace.to_string();
        if (AssertMessage)
        {
            Content.insert(0, "\n================================\n");
            Content.insert(0, AssertMessage);
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
        printf("Crash with signal: %d\n", SigNum);
        if (AssertMessage)
        {
            printf("%s", AssertMessage);
        }
    }
}
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
