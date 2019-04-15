#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISFieldEditBase.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISRadioEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISRadioEdit(QWidget *parent);
	virtual ~ISRadioEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

protected:
	void AddButton(QRadioButton *RadioButton, const QVariant &Value); //Добавить кнопку

private:
	QHBoxLayout *LayoutPanel;
	QWidget *WidgetPanel;
	QButtonGroup *ButtonGroup;
	QMap<int, QVariant> MapButtons;
};
//-----------------------------------------------------------------------------
