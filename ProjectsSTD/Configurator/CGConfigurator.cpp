#include "CGConfigurator.h"
#include "ISLogger.h"
#include "CGConfiguratorBase.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
CGConfigurator::CGConfigurator(const std::vector<CGSection> &arguments)
    : CGConfiguratorBase(),
    Arguments(arguments)
{

}
//-----------------------------------------------------------------------------
CGConfigurator::~CGConfigurator()
{

}
//-----------------------------------------------------------------------------
bool CGConfigurator::help()
{
    ISLOGGER_I(__CLASS__, "Configurator is a utility for configuring the IntegralSystem database");
    ISLOGGER_I(__CLASS__, "Using: Configurator%s [PARAMETER] [FUNCTION_NAME]", EXTENSION_BINARY);
    ISLOGGER_I(__CLASS__, "Using: Configurator%s [PARAMETER]\n", EXTENSION_BINARY);
    for (CGSection Section : Arguments)
    {
        for (CGSectionItem SectionItem : Section.Items)
        {
            ISLOGGER_I("%s %s - %s", Section.Name.c_str(), SectionItem.Function.c_str(), SectionItem.Description.c_str());
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
