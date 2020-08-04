#pragma once
//-----------------------------------------------------------------------------
#include "ISFieldEditBase.h"
#include "ISEdits.h"
//-----------------------------------------------------------------------------
class ISComboEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboEdit(QWidget *parent = 0);
	virtual ~ISComboEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

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
	void HideFirstItem(); //������ ������ ������� � ������

public slots:
	void SetReadOnly(bool read_only);

protected:
	void CurrentIndexChanged(int Index);

private:
	ISQComboBox *ComboBox;
};
//-----------------------------------------------------------------------------
