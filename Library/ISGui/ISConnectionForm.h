#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISConnectionForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISConnectionForm();
	virtual ~ISConnectionForm();

private:
	void AfterShowEvent() override;
	void EnterClicked() override;
	void SaveSettings(); //����������� ���������� ��������
	bool CheckFields(); //�������� �� ���������� �����

private:
	ISLineEdit *EditServer;
	ISIntegerEdit *EditPort;
	ISLineEdit *EditDatabase;
	ISPathEditDir *EditUpdateDir;

	ISCheckEdit *EditProtocolInclude;
	ISIntegerEdit *EditProtocolPort;
};
//-----------------------------------------------------------------------------
