#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISTaskTabBase.h"
#include "ISListWidget.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskHistoryTab : public ISTaskTabBase
{
	Q_OBJECT

public:
	ISTaskHistoryTab(int task_id, QTabWidget *parent = 0);
	virtual ~ISTaskHistoryTab();

	void Update() override;

private:
	ISListWidget *ListWidget;
	QVBoxLayout *LayoutHistory;
	QVector<QWidget*> Vector;
};
//-----------------------------------------------------------------------------
