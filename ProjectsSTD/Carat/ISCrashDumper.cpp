#include "ISCrashDumper.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
#include "ISStackWalker.h"
#else
#endif
//-----------------------------------------------------------------------------
bool ISCrashDumper::Install()
{
#ifdef WIN32
    SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);
    return true;
#else
    return false;
#endif
}
//-----------------------------------------------------------------------------
#ifdef WIN32
LONG ISCrashDumper::CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs)
{
    //Получаем стек
    ISStackWalker stack_walker;
    stack_walker.ShowCallstack(GetCurrentThread(), pExPtrs->ContextRecord);

    //Логируем
    ISLOGGER_C(__CLASS__, "Crash application\n%s", stack_walker.GetStack().c_str());

    //Ждём пока запишется и выходим
    ISSleep(500);
    return EXCEPTION_EXECUTE_HANDLER;
}
//-----------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
