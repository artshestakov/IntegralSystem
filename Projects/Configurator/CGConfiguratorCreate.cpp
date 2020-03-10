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
bool CGConfiguratorCreate::resources()
{
	bool Result = true, Exist = true;
	for (int i = 0, CountResources = ISMetaData::GetInstanse().GetResources().size(); i < CountResources; ++i)
	{
		PMetaClassResource *MetaResource = ISMetaData::GetInstanse().GetResources().at(i);
		Progress("Resources for " + MetaResource->TableName, i, CountResources);

		if (CGResource::CheckExistResource(MetaResource, Exist, ErrorString))
		{
			if (!Exist)
			{
				Result = CGResource::InsertResource(MetaResource, ErrorString);
			}
		}
		
		if (!Result)
		{
			break;
		}
	}
	return Result;
}
//-----------------------------------------------------------------------------
