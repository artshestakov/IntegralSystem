#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISFieldEditBase.h"
#include "ISPhoneLineEdit.h"
#include "ISServiceButton.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISPhoneBaseEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPhoneBaseEdit(QWidget *parent);
	virtual ~ISPhoneBaseEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;
	void SetVisibleClear(bool Visible) override;

	bool IsValid() const override; //�������� ������������ ������
	void SetVisibleCall(bool Visible); //������� ��������� ������ ������
	void SetToolTipCall(const QString &ToolTip);
	void SetCursorCall(const QCursor &Cursor);

protected:
	virtual void Call();
	void PhoneChanged();
	void Paste(const QString &PastedString); //��������� �������

private:
	ISPhoneLineEdit *PhoneLineEdit;
	ISServiceButton *ButtonCall;
};
//-----------------------------------------------------------------------------
