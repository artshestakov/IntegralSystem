#include "StdAfx.h"
#include "CGConfiguratorUpdate.h"
#include "ISDefines.h"
#include "ISAssert.h"
#include "ISDebug.h"
#include "ISSystem.h"
#include "ISMetaData.h"
#include "CGFunction.h"
#include "CGTable.h"
#include "CGIndex.h"
#include "CGForeign.h"
#include "CGResource.h"
#include "CGClass.h"
#include "CGClassField.h"
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
	classes();
	classfields();
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::functions()
{
	ISDebug::ShowDebugString("Updating functions...");

	int CountFunctions = ISMetaData::GetInstanse().GetFunctions().count();
	for (int i = 0; i < CountFunctions; ++i)
	{
		Progress("Functions", i, CountFunctions);

		PMetaClassFunction *MetaFunction = ISMetaData::GetInstanse().GetFunctions().at(i);
		QString ErrorString;
		CGFunction::CreateOrReplaceFunction(MetaFunction, ErrorString);
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::tables()
{
	ISDebug::ShowDebugString("Updating tables...");

	int CountTables = ISMetaData::GetInstanse().GetTables().count();
	for (int i = 0; i < CountTables; ++i) //Обход таблиц
	{
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetTables().at(i);
		Progress(QString("Table: %1").arg(MetaTable->GetName()), i, CountTables);

		QString ErrorString;
		if (CGTable::CheckExistTable(MetaTable)) //Если таблица существует, обновить её, иначе - создать
		{
            CGTable::UpdateTable(MetaTable);
		}
		else
		{
            CGTable::CreateTable(MetaTable, ErrorString);
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::systemindexes()
{
	ISDebug::ShowDebugString("Updating system indexes...");

	int CountIndexes = ISMetaData::GetInstanse().GetSystemIndexes().count();
	for (int i = 0; i < CountIndexes; ++i) //Обход индексов
	{
		PMetaClassIndex *MetaIndex = ISMetaData::GetInstanse().GetSystemIndexes().at(i);
		Progress(QString("System indexes for: %1").arg(MetaIndex->GetTableName()), i, CountIndexes);
		
		QString ErrorString;
		if (CGIndex::CheckExistIndex(MetaIndex))
		{
			if (MetaIndex->GetFieldName().toLower() == "id") //Если поле primary_key - делать reindex
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
	ISDebug::ShowDebugString("Updating indexes...");

	int CountIndexes = ISMetaData::GetInstanse().GetIndexes().count();
	for (int i = 0; i < CountIndexes; ++i) //Обход индексов
	{
		PMetaClassIndex *MetaIndex = ISMetaData::GetInstanse().GetIndexes().at(i);
		Progress(QString("Indexes for: %1").arg(MetaIndex->GetTableName()), i, CountIndexes);

		QString ErrorString;
		if (CGIndex::CheckExistIndex(MetaIndex))
		{
            CGIndex::UpdateIndex(MetaIndex, ErrorString);
		}
		else
		{
            CGIndex::CreateIndex(MetaIndex, ErrorString);
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::compoundindexes()
{
	ISDebug::ShowDebugString("Updating compound indexes...");

	int CountIndexes = ISMetaData::GetInstanse().GetCompoundIndexes().count();
	for (int i = 0; i < CountIndexes; ++i)
	{
		PMetaClassIndex *MetaIndex = ISMetaData::GetInstanse().GetCompoundIndexes().at(i);
		Progress(QString("Compound indexes for: %1").arg(MetaIndex->GetTableName()), i, CountIndexes);

		QString ErrorString;
		if (CGIndex::CheckExistIndex(MetaIndex))
		{
            CGIndex::UpdateIndex(MetaIndex, ErrorString);
		}
		else
		{
            CGIndex::CreateIndex(MetaIndex, ErrorString);
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::foreigns()
{
	ISDebug::ShowDebugString("Updating foreigns...");

	int CountForeigns = ISMetaData::GetInstanse().GetForeigns().count();
	for (int i = 0; i < CountForeigns; ++i)
	{
		Progress("Foreigns", i, CountForeigns);

		PMetaClassForeign *MetaForeign = ISMetaData::GetInstanse().GetForeigns().at(i);
		QString ErrorString;

		if (CGForeign::CheckExistForeign(MetaForeign))
		{
            CGForeign::UpdateForeign(MetaForeign, ErrorString);
		}
		else
		{
            CGForeign::CreateForeign(MetaForeign, ErrorString);
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::resources()
{
	ISDebug::ShowDebugString("Updating resources...");

	int CountResources = ISMetaData::GetInstanse().GetResources().count();
	for (int i = 0; i < CountResources; ++i)
	{
		PMetaClassResource *MetaResource = ISMetaData::GetInstanse().GetResources().at(i);
		Progress(QString("Resources for: %1").arg(MetaResource->GetTableName()), i, CountResources);
		
		QString ErrorString;
		if (CGResource::CheckExistResource(MetaResource))
		{
			CGResource::UpdateResource(MetaResource);
		}
		else
		{
			CGResource::InsertResource(MetaResource, ErrorString);
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::classes()
{
	ISDebug::ShowDebugString("Updating class...");

	int CountTables = ISMetaData::GetInstanse().GetTables().count();
	for (int i = 0; i < ISMetaData::GetInstanse().GetTables().count(); ++i)
	{
		Progress("Classes", i, CountTables);

		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetTables().at(i);
		if (CGClass::CheckExistClass(MetaTable))
		{
			CGClass::UpdateClass(MetaTable);
		}
		else
		{
			CGClass::InsertClass(MetaTable);
		}
	}
}
//-----------------------------------------------------------------------------
void CGConfiguratorUpdate::classfields()
{
	ISDebug::ShowDebugString("Updation class fields...");

	int CountTables = ISMetaData::GetInstanse().GetTables().count();
	for (int i = 0; i < ISMetaData::GetInstanse().GetTables().count(); ++i)
	{
		Progress("Class fields", i, CountTables);
		
		PMetaClassTable *MetaTable = ISMetaData::GetInstanse().GetTables().at(i);

		for (int i = 0; i < MetaTable->GetFields().count(); ++i)
		{
			PMetaClassField *MetaField = MetaTable->GetFields().at(i);
			if (CGClassField::CheckExistClassField(MetaField))
			{
				CGClassField::UpdateClassField(MetaTable->GetUID(), MetaField);
			}
			else
			{
				CGClassField::InsertClassField(MetaTable->GetUID(), MetaField);
			}
		}
	}
}
//-----------------------------------------------------------------------------
