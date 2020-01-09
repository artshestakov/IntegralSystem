#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISSironaCostListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSironaCostListForm(QWidget *parent = 0);
	virtual ~ISSironaCostListForm();

protected:
	void LoadDataAfterEvent() override;

private:
	QLabel *LabelSum;
};
//-----------------------------------------------------------------------------
