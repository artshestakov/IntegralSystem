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
	void AddObjectForm(QWidget *ObjectFormWidget); //�������� ����� ������� �� ������ ���������

protected:
	void CreateSystems(); //�������� ������ ������ � ���������
	void CreateTabWidget(); //������� ����

	void ClickedSubSystem(const QString &SubSystemUID, const QIcon &IconSubSystem); //���������� ������� ����� �� ����������
	void CreateDuplicate(PMetaTable *MetaTable);

private:
	QVBoxLayout *Layout;
	ISTabWidgetMain *TabWidget;
	ISInterfaceMetaForm *CentralForm; //����������� ����� (�������)
	QString CurrentSubSystemUID;
};
//-----------------------------------------------------------------------------
