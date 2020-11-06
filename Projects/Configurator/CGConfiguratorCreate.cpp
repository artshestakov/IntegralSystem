#include "CGConfiguratorCreate.h"
#include "ISAssert.h"
#include "ISDatabase.h"
#include "ISQuery.h"
#include "ISMetaData.h"
#include "ISConfig.h"
#include "ISConstants.h"
#include "ISMetaData.h"
#include "ISMetaDataHelper.h"
#include "ISConsole.h"
#include "CGDatabase.h"
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
	for (size_t i = 0, CountResources = ISMetaData::Instance().GetResources().size(); i < CountResources; ++i)
	{
		PMetaResource *MetaResource = ISMetaData::Instance().GetResources().at(i);
		Progress("Resources for " + MetaResource->TableName, i, CountResources);

		if (CGDatabase::Resource_Exist(MetaResource, Exist, ErrorString))
		{
			if (!Exist)
			{
				Result = CGDatabase::Resource_Insert(MetaResource, ErrorString);
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
