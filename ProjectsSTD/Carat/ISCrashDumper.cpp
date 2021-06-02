#include "ISCrashDumper.h"
#include "ISLogger.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
#ifdef WIN32
#include "ISStackWalker.h"
#else
#endif
//-----------------------------------------------------------------------------
void ISCrashDumper::Install()
{
#ifdef WIN32
    SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);
#else
    signal(SIGSEGV, OnSystemSignal);
    signal(SIGTERM, OnSystemSignal);
    signal(SIGABRT, OnSystemSignal);
    signal(SIGINT, OnSystemSignal);
#endif
}
//-----------------------------------------------------------------------------
#ifdef WIN32
LONG ISCrashDumper::CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs)
{
    //�������� ����
    ISStackWalker stack_walker;
    stack_walker.ShowCallstack(GetCurrentThread(), pExPtrs->ContextRecord);

    //��������
    ISLOGGER_C(__CLASS__, "Crash application\n%s", stack_walker.GetStack().c_str());

    //��� ���� ��������� � �������
    ISSleep(500);
    exit(EXIT_FAILURE);

    return EXCEPTION_EXECUTE_HANDLER;
}
//-----------------------------------------------------------------------------
#else
void ISCrashDumper::OnSystemSignal(int Signal)
{
    //�������� ����
    void *Stack[100] = { 0 };
    size_t StackSize = backtrace(Stack, 100);
    char **Messages = backtrace_symbols(Stack, StackSize);

    //������� ����������
    std::string PathApplication = ISAlgorithm::GetApplicationPath();
    char Buffer[256] = { 0 }, Addr2lineCmd[512] = { 0 };
    std::string StackString;

    //������� ����
    for (size_t i = 0; i < StackSize; ++i)
    {
        //�������� ������� ��������� � ����� �� ������
        char *Message = Messages[i];

        size_t Size = 0;
        ISVectorString VectorString = ISAlgorithm::StringSplit(Message, ' ', Size);
        if (Size != 2) //���-�� ����� �� ���
        {
            std::cout << "Invalid message format: " << Message << std::endl;
            continue;
        }

        //��������� ����� ���������
        std::string Pointer = VectorString.back();
        Pointer = Pointer.substr(1, Pointer.size() - 2);

        //��������� �������
        sprintf(Addr2lineCmd, "addr2line --pretty-print --functions --exe=%s %s",
                PathApplication.c_str(), Pointer.c_str());

        //��������� �������
        FILE *File = popen(Addr2lineCmd, "r");
        if (!File)
        {
            std::cout << ISAlgorithm::StringF("Error exec '%s': %s",
                Addr2lineCmd, ISAlgorithm::GetLastErrorS().c_str()) << std::endl;
            continue;
        }

        //������� ��������� ���������� ������� � �������� ���
        while (fgets(Buffer, sizeof(Buffer), File) != NULL)
        {
            StackString += Buffer;
            //StackString.push_back('\n');
        }
        pclose(File); //��������� �����
    }

    //�������� ����
    ISLOGGER_C(__CLASS__, "Crash application (%s - %d)\n%s",
        strsignal(Signal), Signal, StackString.c_str());

    //��� ���� ��������� � �������
    ISSleep(500);
    exit(EXIT_FAILURE);
}
#endif
//-----------------------------------------------------------------------------
