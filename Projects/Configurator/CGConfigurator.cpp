#include "CGConfigurator.h"
#include "EXDefines.h"
#include "ISConstants.h"
#include "ISAssert.h"
#include "ISSystem.h"
#include "ISDatabase.h"
#include "ISDebug.h"
#include "ISLocalization.h"
#include "ISConfig.h"
#include "CGConfiguratorBase.h"
#include "ISQueryException.h"
#include "ISCountingTime.h"
#include "ISCore.h"
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
	ISDebug::ShowString(QString("Using: Configurator%1 [PARAMETER] [FUNCTION_NAME]").arg(EXTENSION_EXECUTE_BINARY));
	ISDebug::ShowString(QString("Using: Configurator%1 [PARAMETER]").arg(EXTENSION_EXECUTE_BINARY));
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
