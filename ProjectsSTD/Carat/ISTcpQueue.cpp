#include "ISTcpQueue.h"
#include "ISConstants.h"
#include "ISAlgorithm.h"
//-----------------------------------------------------------------------------
ISTcpQueue::ISTcpQueue()
    : IsActive(true)
{
    CRITICAL_SECTION_INIT(&CriticalSectionM);
    CRITICAL_SECTION_INIT(&CriticalSectionA);
}
//-----------------------------------------------------------------------------
ISTcpQueue::~ISTcpQueue()
{
    CRITICAL_SECTION_DESTROY(&CriticalSectionM);
    CRITICAL_SECTION_DESTROY(&CriticalSectionA);
}
//-----------------------------------------------------------------------------
ISTcpQueue& ISTcpQueue::Instance()
{
    static ISTcpQueue TcpQueue;
    return TcpQueue;
}
//-----------------------------------------------------------------------------
void ISTcpQueue::Shutdown()
{
    CRITICAL_SECTION_LOCK(&CriticalSectionM);
    IsActive = false;
    while (!QueueM.empty())
    {
        ISTcpMessage *TcpMessage = QueueM.front();
        QueueM.pop();
        delete TcpMessage;
    }
    CRITICAL_SECTION_UNLOCK(&CriticalSectionM);
}
//-----------------------------------------------------------------------------
void ISTcpQueue::AddMessage(ISTcpMessage *TcpMessage)
{
    CRITICAL_SECTION_LOCK(&CriticalSectionM);
    if (IsActive) //Если очередь активна - добавляем сообщение в неё
    {
        QueueM.push(TcpMessage);
    }
    else //Иначе удаляем
    {
        delete TcpMessage;
    }
    CRITICAL_SECTION_UNLOCK(&CriticalSectionM);
}
//-----------------------------------------------------------------------------
ISTcpMessage* ISTcpQueue::GetMessage()
{
    ISTcpMessage *TcpMessage = nullptr;
    //Блокируем критическую секцию, забираем очередное сообщение и разблокируем секцию
    CRITICAL_SECTION_LOCK(&CriticalSectionM);
    if (IsActive) //Если очередь активна - возвращем очередное сообщение
    {
        if (!QueueM.empty())
        {
            TcpMessage = QueueM.front();
            QueueM.pop();
        }
    }
    CRITICAL_SECTION_UNLOCK(&CriticalSectionM);
    return TcpMessage;
}
//-----------------------------------------------------------------------------
void ISTcpQueue::AddAnswer(ISTcpAnswer *TcpAnswer)
{
    CRITICAL_SECTION_LOCK(&CriticalSectionA);
    if (IsActive)
    {
        QueueA.push(TcpAnswer);
    }
    else
    {
        delete TcpAnswer;
    }
    CRITICAL_SECTION_UNLOCK(&CriticalSectionA);
}
//-----------------------------------------------------------------------------
ISTcpAnswer* ISTcpQueue::GetAnswer()
{
    ISTcpAnswer *TcpAnswer = nullptr;
    CRITICAL_SECTION_LOCK(&CriticalSectionA);
    if (IsActive)
    {
        if (!QueueA.empty())
        {
            TcpAnswer = QueueA.front();
            QueueA.pop();
        }
    }
    CRITICAL_SECTION_UNLOCK(&CriticalSectionA);
    return TcpAnswer;
}
//-----------------------------------------------------------------------------
