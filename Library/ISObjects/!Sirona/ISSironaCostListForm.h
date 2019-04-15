#pragma once
//-----------------------------------------------------------------------------
#include "isobjects_global.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISOBJECTS_EXPORT ISSironaCostListForm : public ISListBaseForm
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
