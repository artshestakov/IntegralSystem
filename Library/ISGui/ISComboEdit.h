#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISFieldEditBase.h"
#include "ISQComboBox.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISComboEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboEdit(QWidget *parent);
	virtual ~ISComboEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

	QString GetCurrentText() const;
	void SetEditable(bool Editable);
	void SetCurrentItem(int Index);
	void SetCurrentText(const QString &Text);
	void InsertItem(int Index, const QString &Text, const QVariant &UserData);
	void AddItem(const QIcon &IconItem, const QString &Text, const QVariant &UserData);
	void AddItem(const QString &Text, const QVariant &UserData);
	void AddItem(const QString &Text);
	void SetIcon(int Index, const QIcon &Icon);
	void SetWheelScroll(bool WheelScroll);
	void SetDuplicatesEnabled(bool Enabled);
	void HideFirstItem(); //Скрыть первый элемент в списке

public slots:
	void SetReadOnly(bool read_only);

protected:
	void CurrentIndexChanged(int Index);

private:
	ISQComboBox *ComboBox;
};
//-----------------------------------------------------------------------------
