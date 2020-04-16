#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEditBase.h"
#include "ISEdits.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISPhoneBaseEdit : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPhoneBaseEdit(QWidget *parent);
	virtual ~ISPhoneBaseEdit();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

	bool IsValid() const override; //Проверка корректности номера
	void SetVisibleCall(bool Visible); //Именить видимость кнопки звонка
	void SetToolTipCall(const QString &ToolTip);
	void SetCursorCall(const QCursor &Cursor);

protected:
	virtual void Call();
	void PhoneChanged();
	void Paste(const QString &PastedString); //Обработка вставки

private:
	ISPhoneLineEdit *PhoneLineEdit;
	ISServiceButton *ButtonCall;
};
//-----------------------------------------------------------------------------
