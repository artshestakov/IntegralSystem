#include "ISLoggerUDP.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
ISLoggerUDP::ISLoggerUDP()
    : ErrorString(STRING_NO_ERROR),
    Flag(ISNamespace::Started::Unknown),
    HandlePipe(nullptr),
    ClientActive(false)
{
    CRITICAL_SECTION_INIT(&CS);
}
//-----------------------------------------------------------------------------
ISLoggerUDP::~ISLoggerUDP()
{
    CRITICAL_SECTION_DESTROY(&CS);
}
//-----------------------------------------------------------------------------
ISLoggerUDP& ISLoggerUDP::Instance()
{
    static ISLoggerUDP LoggerUDP;
    return LoggerUDP;
}
//-----------------------------------------------------------------------------
const std::string& ISLoggerUDP::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
bool ISLoggerUDP::Init()
{
    Flag = ISNamespace::Started::Starting;

    //��������� �����
    std::thread(&ISLoggerUDP::Worker, this).detach();
    
    //��� ��������� �������
    while (true)
    {
        ISSleep(1);
        ISNamespace::Started f = ISNamespace::Started::Unknown;
        CRITICAL_SECTION_LOCK(&CS);
        f = Flag;
        CRITICAL_SECTION_UNLOCK(&CS);

        if (f != ISNamespace::Started::Starting)
        {
            break;
        }
    }
    return Flag == ISNamespace::Started::Started;
}
//-----------------------------------------------------------------------------
void ISLoggerUDP::Shutdown()
{
    if (Flag == ISNamespace::Started::Started && HandlePipe != INVALID_HANDLE_VALUE)
    {
        DisconnectNamedPipe(HandlePipe);
        CloseHandle(HandlePipe);
    }
}
//-----------------------------------------------------------------------------
void ISLoggerUDP::Worker()
{
    HandlePipe = CreateNamedPipe(LOG_PIPE_NAME, PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
        /*PIPE_UNLIMITED_INSTANCES*/1, 512, 512, LOG_PIPE_TIMEOUT, NULL);
    if (HandlePipe == INVALID_HANDLE_VALUE) //�� ������� ������� ����� - ������
    {
        ErrorString = ISAlgorithm::GetLastErrorS();
        CRITICAL_SECTION_LOCK(&CS);
        Flag = ISNamespace::Started::Error;
        CRITICAL_SECTION_UNLOCK(&CS);
        return;
    }

    CRITICAL_SECTION_LOCK(&CS);
    Flag = ISNamespace::Started::Started;
    CRITICAL_SECTION_UNLOCK(&CS);

    OVERLAPPED OverLapped = { 0 };
    BOOL Result = FALSE;
    ISErrorNumber ErrorNumber = 0;
    DWORD Writed = 0;

    while (true)
    {
        ISSleep(1);

        Result = ConnectNamedPipe(HandlePipe, &OverLapped);
        ErrorNumber = ISAlgorithm::GetLastErrorN();
        if (Result == FALSE && ErrorNumber == ERROR_PIPE_LISTENING) //�������� ����������� �������
        {
            continue;
        }
        /*else if (Result == FALSE && ErrorNumber != ERROR_PIPE_LISTENING) //������
        {
            ErrorString = ISAlgorithm::GetLastErrorS();
            break;
        }*/

        CRITICAL_SECTION_LOCK(&CS);
        ClientActive = true;
        CRITICAL_SECTION_UNLOCK(&CS);

        while (true)
        {
            CRITICAL_SECTION_LOCK(&CS);
            //���������� � ����� ��������� �� ������� ���� ������� �� �������� � ���� �������� ������
            while (!Queue.empty() && ClientActive)
            {
                //����������� ��������� ���������
                std::string Message = Queue.front();
                Queue.pop();

                //����������
                if (WriteFile(HandlePipe, Message.c_str(), Message.size(), &Writed, NULL) == FALSE &&
                    Writed == 0) //�� ������� �������� ������ - ����� ������� ��� ������ ����������
                {
                    ClientActive = false;
                    break;
                }
            }
            bool client_active = ClientActive;
            CRITICAL_SECTION_UNLOCK(&CS);
            ISSleep(10); //������� ������

            if (!client_active) //������ ���������� - ��������� � ����� �������� �����������
            {
                break;
            }
        }
    }
}
//-----------------------------------------------------------------------------
void ISLoggerUDP::Add(const std::string &String)
{
    CRITICAL_SECTION_LOCK(&CS);
    if (ClientActive) //���� ������ ������� - ������� ��������� � �������
    {
        Queue.push(String);
    }
    CRITICAL_SECTION_UNLOCK(&CS);
}
//-----------------------------------------------------------------------------
