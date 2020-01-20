#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISProgressForm : public QProgressDialog
{
	Q_OBJECT

public:
	ISProgressForm(int Minimum, int Maximum, QWidget *parent);
	virtual ~ISProgressForm();

	void SetText(const QString &Text); //�������� ����� �� �����
	void AddOneValue(); //��������� �������� �� ���� �������

protected:
	void showEvent(QShowEvent *e);

private:
	QLabel *LabelStatus;
	int Value;
};
//-----------------------------------------------------------------------------
