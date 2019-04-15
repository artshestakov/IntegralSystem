#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISInterfaceDialogForm.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISAboutForm : public ISInterfaceDialogForm
{
	Q_OBJECT

public:
	ISAboutForm(QWidget *parent = 0);
	virtual ~ISAboutForm();

protected:
	void CreateCommonTab(); //�����
	void CreateContactsTab(); //��������
	void CreateModuleTab(); //������� ������
	void CreateDescriptionTab(); //��������
	void CreateLicenseTab(); //������������ ����������
	void CreateShortcuts(); //���������� ������
	void CreateOtherTab(); //������

	void AddLabel(QWidget *parent, const QString &LabelText, const QString &Text);
	void SaveAuthorInfo(); //��������� ���������� � ������������

private:
	QVBoxLayout *LayoutRight;
	QTabWidget *TabWidget;
};
//-----------------------------------------------------------------------------
