#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListViewWidget.h"
#include "ISUserEdit.h"
#include "ISDateEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCallUserStatisticsForm : public ISListViewWidget
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCallUserStatisticsForm(QWidget *parent = 0);
	virtual ~ISCallUserStatisticsForm();

protected:
	void FilterChanged();

private:
	ISUserEdit *UserEdit;
	ISDateEdit *DateEdit;
};
//-----------------------------------------------------------------------------
