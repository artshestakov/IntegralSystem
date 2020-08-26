#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISRangeEdits.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISAsteriskTotalForm : public ISInterfaceForm
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
