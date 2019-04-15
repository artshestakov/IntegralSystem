#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISInterfaceForm.h"
#include "ISRangeDateTimeEdit.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISAsteriskTotalForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISAsteriskTotalForm(QWidget *parent = 0);
	virtual ~ISAsteriskTotalForm();

protected:
	void CreateTotal();
	void CreateUsers();

	void Update();
	void ReloadTotal();
	void ReloadUsers();

private:
	ISRangeDateTimeEdit *RangeDateTimeEdit;
	ISPushButton *ButtonRange;

	QLabel *LabelCountTotal; //����� �������
	QLabel *LabelCountIncoming; //��������
	QLabel *LabelCountOutcoming; //���������

	QLabel *LabelDurationTotal; //����� ������������
	QLabel *LabelDurationIncoming; //������������ ��������
	QLabel *LabelDurationOutcoming; //������������ ���������

	QLabel *LabelAverageIncoming; //������� ����������������� ���������
	QLabel *LabelAverageOutcoming; //������� ����������������� ��������

	QTreeWidget *TreeWidget;
};
//-----------------------------------------------------------------------------
