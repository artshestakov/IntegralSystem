#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISPassportEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISPassportEdit(QWidget *parent);
	virtual ~ISPassportEdit();

	void SetValue(const QVariant &value);
	QVariant GetValue() const override;
	void Clear() override;

protected:
	virtual void Edit(); //����� ������ ����������� �� ������ ����� �������� ����� (����������� �������, �������� ������ �����)
	QString PreparePassport(const QString &passport_string);

private:
	QString PassportString;
};
//-----------------------------------------------------------------------------
