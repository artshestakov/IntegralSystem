#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISFieldEditBase.h"
#include "ISQDoubleSpinBox.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDoubleEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDoubleEdit(QWidget *parent);
	virtual ~ISDoubleEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

	void SetMinimum(double Minimum);
	void SetMaximum(double Maximum);
	void SetSuffix(const QString &Suffix);

public slots:
	void SetReadOnly(bool read_only);

protected:
	void OnValueChanged(const QString &Text);

private:
	ISQDoubleSpinBox *DoubleSpinBox;
	QString StringValue;
	double DoubleValue;
};
//-----------------------------------------------------------------------------
