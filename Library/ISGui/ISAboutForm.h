#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISAboutForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAboutForm();
	virtual ~ISAboutForm();

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
