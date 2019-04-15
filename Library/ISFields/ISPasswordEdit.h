#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISLineEdit.h"
#include "ISServiceButton.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISPasswordEdit : public ISLineEdit
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
