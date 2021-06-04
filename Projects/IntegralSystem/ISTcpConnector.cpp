#include "ISTcpConnector.h"
#include "ISConstantsOld.h"
#include "ISAlgorithmOld.h"
#include "ISLocalizationOld.h"
#include "ISLoggerOld.h"
//-----------------------------------------------------------------------------
ISTcpConnector::ISTcpConnector()
    : QObject(),
    ErrorString(NO_ERROR_STRING),
    TcpSocket(new QTcpSocket(this)),
    HandlingError(true)
{
    connect(TcpSocket, static_cast<void(QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error), this, &ISTcpConnector::Error);
}
//-----------------------------------------------------------------------------
ISTcpConnector::~ISTcpConnector()
{

}
//-----------------------------------------------------------------------------
ISTcpConnector& ISTcpConnector::Instance()
{
    static ISTcpConnector TcpConnector;
    return TcpConnector;
}
//-----------------------------------------------------------------------------
QString ISTcpConnector::GetErrorString() const
{
    return ErrorString;
}
//-----------------------------------------------------------------------------
QTcpSocket* ISTcpConnector::GetSocket()
{
    return TcpSocket;
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::IsConnected() const
{
    return TcpSocket->state() == QTcpSocket::ConnectedState;
}
//-----------------------------------------------------------------------------
bool ISTcpConnector::Connect(const QString &Host, quint16 Port)
{
    if (IsConnected()) //Если подключение поднято - возвращаем true
    {
        return true;
    }

    //Объявляем таумауты
    size_t Timeout = CARAT_CONNECT_TIMEOUT,
        SleepTime = CARAT_CONNECT_SLEEP;

    //Пытаемся подключиться и ждём пока не подключимся
    ISLOGGER_I(__CLASS__, QString("Connecting to %1:%2...").arg(Host).arg(Port));
    TcpSocket->connectToHost(Host, Port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    while (!IsConnected())
    {
        Timeout -= SleepTime;
        ISSleep(SleepTime);
        PROCESS_EVENTS();
        if (!Timeout) //Таймаут истёк
        {
            TcpSocket->disconnectFromHost();
            ErrorString = LANG("TcpConnector.Timeout");
            break;
        }
    }

    if (!IsConnected())
    {
        ISLOGGER_E(__CLASS__, "Not connected with timeout");
        return false;
    }
    ISLOGGER_I(__CLASS__, "Connected");
    ErrorString = NO_ERROR_STRING;
    HandlingError = true; //Устанавливаем разрешение на обработку ошибок
    return true;
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Disconnect()
{
    if (IsConnected()) //Если подключение активно - отключаемся
    {
        ISLOGGER_I(__CLASS__, "Disconnecting...");
        TcpSocket->disconnectFromHost();
    }
}
//-----------------------------------------------------------------------------
void ISTcpConnector::Error(QTcpSocket::SocketError socket_error)
{
    if (HandlingError) //Если обработка ошибок включена - обрабатываем
    {
        ErrorString = TcpSocket->errorString();
        ISLOGGER_E(__CLASS__, ErrorString);
        if (socket_error == QAbstractSocket::SocketError::RemoteHostClosedError) //Удалённый хост оборвал соединение - рапортуем о необходимости переподключения
        {
            QTimer::singleShot(100, this, &ISTcpConnector::Reconnect);
            ISLOGGER_I(__CLASS__, "Emited reconnect");
            HandlingError = false;
        }
    }
}
//-----------------------------------------------------------------------------
