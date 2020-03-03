#include "CGConfiguratorUpdate.h"
#include "ISMetaData.h"
#include "CGFunction.h"
#include "CGTable.h"
#include "CGIndex.h"
#include "CGForeign.h"
#include "CGResource.h"
#include "ISQuery.h"
#include "ISConfig.h"
#include "ISLogger.h"
//-----------------------------------------------------------------------------
CGConfiguratorUpdate::CGConfiguratorUpdate() : CGConfiguratorBase()
{

}
//-----------------------------------------------------------------------------
CGConfiguratorUpdate::~CGConfiguratorUpdate()
{

}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::database()
{
	functions();
	tables();
	systemindexes();
	indexes();
	compoundindexes();
	foreigns();
	resources();
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::functions()
{
	ISLOGGER_DEBUG("Updating functions...");
	for (int i = 0, CountFunctions = ISMetaData::GetInstanse().GetFunctions().count(); i < CountFunctions; ++i)
	{
		Progress("Functions", i, CountFunctions);
		QString ErrorString;
		CGFunction::CreateOrReplaceFunction(ISMetaData::GetInstanse().GetFunctions()[i], ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::tables()
{
	ISLOGGER_DEBUG("Updating tables...");
	for (int i = 0, CountTables = ISMetaData::GetInstanse().GetTables().count(); i < CountTables; ++i) //Обход таблиц
	{
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetTables().at(i);
		Progress(QString("Table: %1").arg(MetaTable->Name), i, CountTables);

		QString ErrorString;
		CGTable::CheckExistTable(MetaTable) ? CGTable::UpdateTable(MetaTable) : CGTable::CreateTable(MetaTable, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::systemindexes()
{
	ISLOGGER_DEBUG("Updating system indexes...");
	for (int i = 0, CountIndexes = ISMetaData::GetInstanse().GetSystemIndexes().count(); i < CountIndexes; ++i) //Обход индексов
	{
		PMetaClassIndex *MetaIndex = ISMetaData::GetInstanse().GetSystemIndexes().at(i);
		Progress(QString("System indexes for: %1").arg(MetaIndex->TableName), i, CountIndexes);
		
		QString ErrorString;
		if (CGIndex::CheckExistIndex(MetaIndex))
		{
			if (MetaIndex->FieldName.toLower() == "id") //Если поле primary_key - делать reindex
			{
                CGIndex::ReindexIndex(MetaIndex, ErrorString);
			}
			else if (CGIndex::CheckIndexForeign(MetaIndex)) //Если на поле, где установлен текущий индекс ссылается внешний ключ - делать reindex
			{
                CGIndex::ReindexIndex(MetaIndex, ErrorString);
			}
			else
			{
                CGIndex::UpdateIndex(MetaIndex, ErrorString);
			}
		}
		else
		{
            CGIndex::CreateIndex(MetaIndex, ErrorString);
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::indexes()
{
	ISLOGGER_DEBUG("Updating indexes...");
	for (int i = 0, CountIndexes = ISMetaData::GetInstanse().GetIndexes().count(); i < CountIndexes; ++i) //Обход индексов
	{
		PMetaClassIndex *MetaIndex = ISMetaData::GetInstanse().GetIndexes().at(i);
		Progress(QString("Indexes for: %1").arg(MetaIndex->TableName), i, CountIndexes);

		QString ErrorString;
		CGIndex::CheckExistIndex(MetaIndex) ? CGIndex::UpdateIndex(MetaIndex, ErrorString) : CGIndex::CreateIndex(MetaIndex, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::compoundindexes()
{
	ISLOGGER_DEBUG("Updating compound indexes...");
	for (int i = 0, CountIndexes = ISMetaData::GetInstanse().GetCompoundIndexes().count(); i < CountIndexes; ++i)
	{
		PMetaClassIndex *MetaIndex = ISMetaData::GetInstanse().GetCompoundIndexes().at(i);
		Progress(QString("Compound indexes for: %1").arg(MetaIndex->TableName), i, CountIndexes);

		QString ErrorString;
		CGIndex::CheckExistIndex(MetaIndex) ? CGIndex::UpdateIndex(MetaIndex, ErrorString) : CGIndex::CreateIndex(MetaIndex, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::foreigns()
{
	ISLOGGER_DEBUG("Updating foreigns...");
	for (int i = 0, CountForeigns = ISMetaData::GetInstanse().GetForeigns().count(); i < CountForeigns; ++i)
	{
		Progress("Foreigns", i, CountForeigns);
		PMetaClassForeign *MetaForeign = ISMetaData::GetInstanse().GetForeigns().at(i);
		QString ErrorString;
		CGForeign::CheckExistForeign(MetaForeign) ? CGForeign::UpdateForeign(MetaForeign, ErrorString) : CGForeign::CreateForeign(MetaForeign, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::resources()
{
	ISLOGGER_DEBUG("Updating resources...");
	for (int i = 0, CountResources = ISMetaData::GetInstanse().GetResources().count(); i < CountResources; ++i)
	{
		PMetaClassResource *MetaResource = ISMetaData::GetInstanse().GetResources().at(i);
		Progress(QString("Resources for: %1").arg(MetaResource->TableName), i, CountResources);
		
		QString ErrorString;
		CGResource::CheckExistResource(MetaResource) ? CGResource::UpdateResource(MetaResource) : CGResource::InsertResource(MetaResource, ErrorString);
	}
}
//-----------------------------------------------------------------------------
