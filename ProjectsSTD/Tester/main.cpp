#include "ISAlgorithm.h"
#include "ISLogger.h"
#include "ISDebug.h"
#include "ISConsole.h"
#include "ISConfig.h"
#include "ISTcpConnector.h"
#include "ISTcpQuery.h"
//-----------------------------------------------------------------------------
bool TestConnectDisconnect(const std::string &Host, unsigned short Port)
{
    while (true)
    {
        if (!ISTcpConnector::Instance().Connect(Host, Port))
        {
            ISLOGGER_E("ISTcpConnector", ISTcpConnector::Instance().GetErrorString().c_str());
            return EXIT_FAILURE;
        }
        ISTcpConnector::Instance().Disconnect();
        ISSleep(100);
    }
}
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    IS_UNUSED(argc);
    IS_UNUSED(argv);

    if (!ISLogger::Instance().Initialize()) //Не удалось иницилизировать логгер
    {
        ISDEBUG_E("Error initialize logger: " + ISLogger::Instance().GetErrorString());
        return false;
    }

    std::string ErrorString;
    if (!ISConsole::InstallEncoding(65001, ErrorString))
    {
        ISLOGGER_W("Console", "Not setting console encoding: %s", ErrorString.c_str());
    }

    if (!ISConfig::Instance().Initialize(CONFIG_TEMPLATE_TESTER))
    {
        ISLOGGER_E("ISConfig", "Not initialize: %s", ISConfig::Instance().GetErrorString().c_str());
        return false;
    }

    //Получаем настройки
    std::string Host = ISConfig::Instance().GetValueString("Connection", "Host");
    unsigned short Port = ISConfig::Instance().GetValueUShort("Connection", "Port");
    
    //Подключаемся к серверу
    if (!ISTcpConnector::Instance().Connect(Host, Port))
    {
        ISLOGGER_E("ISTcpConnector", ISTcpConnector::Instance().GetErrorString().c_str());
        return EXIT_FAILURE;
    }

    //Авторизация
    ISTcpQuery qAuth(API_AUTH);
    qAuth.BindString("Login", "postgres");
    qAuth.BindString("Password", "adm777");
    qAuth.Execute();

    //Отключаемся от сервера
    ISTcpConnector::Instance().Disconnect();
    ISConsole::Pause();
    return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
