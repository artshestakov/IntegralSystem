#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTaskInfoWidget : public QWidget
{
	Q_OBJECT

public:
	ISTaskInfoWidget(int task_id, QWidget *parent = 0);
	virtual ~ISTaskInfoWidget();

	void Update();

protected:
	QLabel* AddLabel(const QString &RowText);

private:
	int TaskID;

	QFormLayout *FormLayout;
	QLabel *LabelNumber;
	QLabel *LabelStatus;
	QLabel *LabelPriority;
	QLabel *LabelDeadline;
	QLabel *LabelOwner;
	QLabel *LabelExecutor;
	QLabel *LabelDateTime;
	QLabel *LabelResolution;
};
//-----------------------------------------------------------------------------
