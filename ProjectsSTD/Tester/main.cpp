#include "ISAlgorithm.h"
#include "ISLogger.h"
#include "ISDebug.h"
#include "ISConsole.h"
#include "ISConfig.h"
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

    return 0;
}
//-----------------------------------------------------------------------------
