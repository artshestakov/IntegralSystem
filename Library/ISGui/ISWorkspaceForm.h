#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISParagraphBaseForm.h"
#include "ISTabWidgetMain.h"
#include "ISInterfaceMetaForm.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISWorkspaceForm : public ISParagraphBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISWorkspaceForm(QWidget *parent = 0);
	virtual ~ISWorkspaceForm();

	void Invoke() override;

public slots:
	void AddObjectForm(QWidget *ObjectForm); //�������� ����� ������� �� ������ ���������

protected:
	void CreateSystems(); //�������� ������ ������ � ���������
	void CreateTabWidget(); //������� ����

	void ClickedSubSystem(const QString &SubSystemUID, const QIcon &IconSubSystem); //���������� ������� ����� �� ����������

private:
	QVBoxLayout *Layout;
	ISTabWidgetMain *TabWidget;
	ISInterfaceMetaForm *CentralForm; //����������� ����� (�������)
	QString CurrentSubSystemUID;
};
//-----------------------------------------------------------------------------
