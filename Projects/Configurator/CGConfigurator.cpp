#include "CGConfigurator.h"
#include "ISSystem.h"
#include "ISDebug.h"
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
	ISDebug::ShowString("Configurator is a utility for configuring the IntegralSystem database");
	ISDebug::ShowString(QString("Using: Configurator%1 [PARAMETER] [FUNCTION_NAME]").arg(EXTENSION_BINARY));
	ISDebug::ShowString(QString("Using: Configurator%1 [PARAMETER]").arg(EXTENSION_BINARY));
	ISDebug::ShowEmptyString();
	for (CGSection *Section : Arguments)
	{
		for (CGSectionItem *SectionItem : Section->GetItems())
		{
			ISDebug::ShowString(Section->GetName() + SYMBOL_SPACE + SectionItem->GetFunction() + " - " + SectionItem->GetDescription());
		}
	}
}
//-----------------------------------------------------------------------------
