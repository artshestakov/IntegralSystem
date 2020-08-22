#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISHistoryForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISHistoryForm(QWidget *parent = 0);
	virtual ~ISHistoryForm();

protected:
	void EscapeClicked();

private:
	void Open(QListWidgetItem *ListWidgetItem);
	void CheckEditChanged(const QVariant &Value);
	void Clear();

private:
	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
