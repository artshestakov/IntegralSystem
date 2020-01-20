#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISHistoryForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void OpenObject(const QString &TableName, int ObjectID);

public:
	ISHistoryForm(QWidget *parent = 0);
	virtual ~ISHistoryForm();

protected:
	void Open();

private:
	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
