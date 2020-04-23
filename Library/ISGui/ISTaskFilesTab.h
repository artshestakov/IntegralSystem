#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISTaskTabBase.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISTaskFilesTab : public ISTaskTabBase
{
	Q_OBJECT

public:
	ISTaskFilesTab(int task_id, QTabWidget *parent = 0);
	virtual ~ISTaskFilesTab();

	void Update() override;

private:
	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
