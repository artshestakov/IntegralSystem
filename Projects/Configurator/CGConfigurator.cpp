#include "CGConfigurator.h"
#include "ISSystem.h"
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
	ISLOGGER_UNKNOWN("Configurator is a utility for configuring the IntegralSystem database");
	ISLOGGER_UNKNOWN(QString("Using: Configurator%1 [PARAMETER] [FUNCTION_NAME]").arg(EXTENSION_BINARY));
	ISLOGGER_UNKNOWN(QString("Using: Configurator%1 [PARAMETER]").arg(EXTENSION_BINARY));
	ISLOGGER_EMPTY();
	for (CGSection *Section : Arguments)
	{
		for (CGSectionItem *SectionItem : Section->Items)
		{
			ISLOGGER_UNKNOWN(Section->Name + SYMBOL_SPACE + SectionItem->Function + " - " + SectionItem->Description);
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
