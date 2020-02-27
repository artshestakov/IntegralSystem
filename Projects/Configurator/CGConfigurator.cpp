#include "CGConfigurator.h"
#include "ISSystem.h"
#include "ISLogger.h"
#include "CGConfiguratorBase.h"
#include "ISConstants.h"
//-----------------------------------------------------------------------------
CGConfigurator::CGConfigurator(const QVector<CGSection*> &arguments)
	: CGConfiguratorBase(),
	Arguments(arguments)
{
	
}
//-----------------------------------------------------------------------------
CGConfigurator::~CGConfigurator()
{

}
//-----------------------------------------------------------------------------
void CGConfigurator::help()
{
	ISLOGGER_UNKNOWN("Configurator is a utility for configuring the IntegralSystem database");
	ISLOGGER_UNKNOWN(QString("Using: Configurator%1 [PARAMETER] [FUNCTION_NAME]").arg(EXTENSION_BINARY));
	ISLOGGER_UNKNOWN(QString("Using: Configurator%1 [PARAMETER]").arg(EXTENSION_BINARY));
	ISLOGGER_EMPTY();
	for (CGSection *Section : Arguments)
	{
		for (CGSectionItem *SectionItem : Section->GetItems())
		{
			ISLOGGER_UNKNOWN(Section->GetName() + SYMBOL_SPACE + SectionItem->GetFunction() + " - " + SectionItem->GetDescription());
		}
	}
}
//-----------------------------------------------------------------------------
