#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISIPAddressEdit.h"
#include "ISIntegerEdit.h"
#include "ISCheckEdit.h"
#include "ISPasswordEdit.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class ISConnectionForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISConnectionForm(QWidget *parent = 0);
	virtual ~ISConnectionForm();

protected:
	void AfterShowEvent() override;
	void EnterClicked() override;
	void SaveSettings(); //����������� ���������� ��������
	bool CheckFields(); //�������� �� ���������� �����

private:
	ISIPAddressEdit *EditServer;
	ISIntegerEdit *EditPort;
	ISLineEdit *EditDatabase;
};
//-----------------------------------------------------------------------------
