#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISPasswordEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPasswordEdit(QWidget *parent);
	virtual ~ISPasswordEdit();

	void SetVisibleGenerate(bool Visible); //����������/������������ ������ ��������� ������
	void SetVisibleCheckBox(bool Visible);

protected:
	void PasswordVisibleChanged(int State);
	void GeneratePassword(); //��������� ������

private:
	QCheckBox *CheckPasswordVisible;
	ISServiceButton *ButtonGeneratePassword;
};
//-----------------------------------------------------------------------------
