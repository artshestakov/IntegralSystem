#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISTaskTabBase.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskFilesTab : public ISTaskTabBase
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
