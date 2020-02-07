#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
struct ISSortingMetaTable
{
public:
	ISSortingMetaTable();
	~ISSortingMetaTable();

	QString TableName;
	QString FieldName;
	Qt::SortOrder Sorting;
	bool ModificationFlag;
};
//-----------------------------------------------------------------------------
