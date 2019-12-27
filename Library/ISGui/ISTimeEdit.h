#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISDateTimeEdit.h"
#include "ISServiceButton.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISTimeEdit : public ISDateTimeEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTimeEdit(QWidget *parent);
	virtual ~ISTimeEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;

protected:
	void DateEnableChanged(int State) override;
	void SelectTime();

private:
	ISServiceButton *ButtonTime;
};
//-----------------------------------------------------------------------------
