#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISFieldEditBase.h"
#include "ISQSpinBox.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISIntegerEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISIntegerEdit(QWidget *parent);
	virtual ~ISIntegerEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

	void SetMinimum(int Minimum);
	void SetMaximum(int Maximum);

public slots:
	void SetReadOnly(bool ReadOnly);

protected:
	void OnValueChanged(const QString &Text);

private:
	ISQSpinBox *SpinBox;
	QString IntValue;
};
//-----------------------------------------------------------------------------
