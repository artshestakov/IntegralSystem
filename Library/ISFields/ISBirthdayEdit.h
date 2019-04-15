#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISDateEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISBirthdayEdit : public ISDateEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISBirthdayEdit(QWidget *parent);
	virtual ~ISBirthdayEdit();

	void SetValue(const QVariant &value) override;

protected:
	void UpdateLabel(const QDate &Date);
	void DateEnableChanged(int State) override;

private:
	QLabel *Label;
};
//-----------------------------------------------------------------------------
