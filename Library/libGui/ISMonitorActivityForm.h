#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceMetaForm.h"
#include "ISScrollArea.h"
#include "ISMonitorUserWidget.h"
#include "ISCheckEdit.h"
//-----------------------------------------------------------------------------
class ISMonitorActivityForm : public ISInterfaceMetaForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISMonitorActivityForm(QWidget *parent = 0);
	virtual ~ISMonitorActivityForm();

	void LoadData() override;

protected:
	void CheckEditChanged(const QVariant &value);
	void Search(const QVariant &value);
	void ShowUserCard(); //�������� �������� ������������
	void ShowActivity(); //�������� ����������
	void ShowProtocol(); //�������� ���������
	void ShowDetails(); //�������� ������������
	void TerminateUser(); //��������� �����
	void GetScreenshot(); //�������� ������� ����
	void SendNotify(); //��������� �����������

	void TerminateAll(); //��������� ����
	void ScreenshotCreated(const QVariantMap &VariantMap);
	void CloseScreenshot(QObject *Object);

private:
	ISCheckEdit *CheckEdit;
	QLabel *LabelCount;
	ISScrollArea *ScrollArea;
	QVector<ISMonitorUserWidget*> VectorUsers;
};
//-----------------------------------------------------------------------------
