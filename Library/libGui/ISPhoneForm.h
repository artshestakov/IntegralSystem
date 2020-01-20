#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISPhoneForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISPhoneForm(QWidget *parent = 0);
	virtual ~ISPhoneForm();

	void keyPressEvent(QKeyEvent *e) override;

protected:
	void CreateButton(int ID, int RowIndex, int ColumnIndex, const QString &Text, const QString &AudioResuourceName);
	void BackspaceNumber();
	void ButtonClicked(QAbstractButton *AbstractButton);
	void Call();
	void Hide();

private:
	QSound *Sound;
	ISLineEdit *EditNumber;
	QGridLayout *GridLayout;
	QButtonGroup *ButtonGroup;
};
//-----------------------------------------------------------------------------
