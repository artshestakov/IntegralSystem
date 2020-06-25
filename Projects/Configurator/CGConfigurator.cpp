#include "CGConfigurator.h"
#include "ISLogger.h"
#include "CGConfiguratorBase.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
CGConfigurator::CGConfigurator(const std::vector<CGSection*> &arguments)
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
	ISLOGGER_L("Configurator is a utility for configuring the IntegralSystem database");
	ISLOGGER_L(QString("Using: Configurator%1 [PARAMETER] [FUNCTION_NAME]").arg(EXTENSION_BINARY));
	ISLOGGER_L(QString("Using: Configurator%1 [PARAMETER]").arg(EXTENSION_BINARY));
	ISLOGGER_N();
	for (CGSection *Section : Arguments)
	{
		for (CGSectionItem *SectionItem : Section->Items)
		{
			ISLOGGER_L(Section->Name + SYMBOL_SPACE + SectionItem->Function + " - " + SectionItem->Description);
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
