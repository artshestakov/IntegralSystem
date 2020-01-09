#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISFieldEditBase.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISCheckEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCheckEdit(QWidget *parent);
	virtual ~ISCheckEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

	void SetText(const QString &Text);
	
public slots:
	void SetReadOnly(bool ReadOnly);

protected:
	QCheckBox* GetCheckBox();

private:
	QCheckBox *CheckBox;
};
//-----------------------------------------------------------------------------
