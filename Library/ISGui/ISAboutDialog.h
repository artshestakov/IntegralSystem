#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISAboutDialog : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAboutDialog();
	virtual ~ISAboutDialog();

protected:
	void CreateCommonTab(); //�����
	void CreateContactsTab(); //��������
	void CreateModuleTab(); //������� ������
	void CreateDescriptionTab(); //��������
	void CreateLicenseTab(); //������������ ����������
	void CreateOtherTab(); //������

	void AddLabel(QWidget *parent, const QString &LabelText, const QString &Text);

private:
	QVBoxLayout *LayoutRight;
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
