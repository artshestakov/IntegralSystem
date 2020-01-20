#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISUuid.h"
//-----------------------------------------------------------------------------
class ISToolBarAccessTable : public QToolBar
{
	Q_OBJECT

public:
	ISToolBarAccessTable(int GroupID, const ISUuid &TableUID, const QString &TableName, QWidget *parent = 0);
	virtual ~ISToolBarAccessTable();
};
//-----------------------------------------------------------------------------
