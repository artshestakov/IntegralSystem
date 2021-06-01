#include "CGConfigurator.h"
#include "ISDebug.h"
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
	ISDEBUG_L("Configurator is a utility for configuring the IntegralSystem database");
	ISDEBUG_L(QString("Using: Configurator%1 [PARAMETER] [FUNCTION_NAME]").arg(EXTENSION_BINARY));
	ISDEBUG_L(QString("Using: Configurator%1 [PARAMETER]").arg(EXTENSION_BINARY));
	ISDEBUG();
	for (CGSection *Section : Arguments)
	{
		for (CGSectionItem *SectionItem : Section->Items)
		{
			ISDEBUG_L(Section->Name + SYMBOL_SPACE + SectionItem->Function + " - " + SectionItem->Description);
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
