#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISAsteriskCallsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISAsteriskCallsListForm(QWidget *parent = 0);
	virtual ~ISAsteriskCallsListForm();

protected:
	void CreateDetailsPanel();
	void CreateStatusDescription();

	void DetailsClicked();
	void DoubleClickedTable(const QModelIndex &ModelIndex) override;
	void Back();

	void FillLabels();
	void StatusClicked();

	void DefaultFilter(); //���������� ����������� ������
	void TransferFilter(const QString &UniqueID); //���������� ������ ��� ������������ �������
	ISUuid GetStatusCall(); //�������� ������ ����������� ������

private:
	QAction *ActionDetails;
	QWidget *WidgetCallInfo;
	ISPushButton *ButtonBack;
	int ObjectID;

	QLabel *LabelDateTime;
	QLabel *LabelDirection;
	QLabel *LabelSubscriber;
	QLabel *LabelNumber;
	QLabel *LabelDuration;
	QLabel *LabelStatus;
};
//-----------------------------------------------------------------------------
