#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISFieldEditBase.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISColorEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISColorEdit(QWidget *parent);
	virtual ~ISColorEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

public slots:
	void SetReadOnly(bool read_only);

protected:
	void SelectColor();

private:
	QWidget *WidgetColor;
};
//-----------------------------------------------------------------------------
