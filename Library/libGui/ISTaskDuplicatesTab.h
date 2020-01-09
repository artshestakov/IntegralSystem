#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISTaskTabBase.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTaskDuplicatesTab : public ISTaskTabBase
{
	Q_OBJECT

public:
	ISTaskDuplicatesTab(int task_id, QTabWidget *parent = 0);
	virtual ~ISTaskDuplicatesTab();

	void Update() override;

protected:
	void ItemDoubleClicked(QListWidgetItem *ListWidgetItem);

private:
	ISListWidget *ListWidget;
};
//-----------------------------------------------------------------------------
