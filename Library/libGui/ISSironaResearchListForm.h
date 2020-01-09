#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISSironaResearchListForm : public ISListBaseForm
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
