#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISFieldEditBase.h"
#include "ISQTextEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISTextEdit : public ISFieldEditBase
{
	Q_OBJECT

signals:
	void KeyPressEnter();

public:
	Q_INVOKABLE ISTextEdit(QWidget *parent = 0);
	virtual ~ISTextEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

	void SetReadOnly(bool ReadOnly) override;
	void SetPlaceholderText(const QString &placeholder_text) override;

	void AddText(const QString &Text);
	void SetExecuteEnter(bool Execute);
	void SetUppercase(bool uppercase);
	void SetLowercase(bool lowercase);
	void ResetFontcase(); //����� �������� ��������

protected:
	void OnUpperText();
	void OnLowerText();
	ISQTextEdit* GetTextEdit();

private:
	ISQTextEdit *TextEdit;
};
//-----------------------------------------------------------------------------
