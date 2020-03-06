#include "CGConfiguratorCreate.h"
#include "ISLogger.h"
#include "ISAssert.h"
#include "ISDatabase.h"
#include "ISQuery.h"
#include "CGResource.h"
#include "ISMetaData.h"
#include "ISConfig.h"
#include "ISConstants.h"
#include "ISMetaData.h"
#include "ISMetaDataHelper.h"
#include "ISConsole.h"
//-----------------------------------------------------------------------------
CGConfiguratorCreate::CGConfiguratorCreate() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorCreate::~CGConfiguratorCreate()
{

}
//-----------------------------------------------------------------------------
void CGConfiguratorCreate::resources()
{
	int CountResources = ISMetaData::GetInstanse().GetResources().size();
	for (int i = 0; i < CountResources; ++i)
	{
		PMetaClassResource *MetaResource = ISMetaData::GetInstanse().GetResources().at(i);
		Progress("Resources for " + MetaResource->TableName, i, CountResources);

		QString ErrorString;
		if (!CGResource::CheckExistResource(MetaResource))
		{
			CGResource::InsertResource(MetaResource, ErrorString);
		}
	}
}
//-----------------------------------------------------------------------------
