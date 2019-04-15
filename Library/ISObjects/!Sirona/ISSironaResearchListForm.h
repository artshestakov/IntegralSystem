#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISSironaResearchListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSironaResearchListForm(QWidget *parent = 0);
	virtual ~ISSironaResearchListForm();

protected:
	void LoadDataAfterEvent() override;

private:
	QLabel *LabelSummary;
};
//-----------------------------------------------------------------------------
