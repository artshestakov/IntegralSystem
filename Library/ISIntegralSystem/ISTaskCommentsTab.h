#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISTaskTabBase.h"
#include "ISScrollArea.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskCommentsTab : public ISTaskTabBase
{
	Q_OBJECT

public:
	ISTaskCommentsTab(int task_id, QTabWidget *parent = 0);
	virtual ~ISTaskCommentsTab();

	void Update() override;

private:
	ISScrollArea *ScrollAreaComments;
	QVBoxLayout *LayoutComments;
	QVector<QWidget*> Vector;
};
//-----------------------------------------------------------------------------
