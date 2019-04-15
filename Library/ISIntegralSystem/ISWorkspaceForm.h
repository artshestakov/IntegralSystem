#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISParagraphBaseForm.h"
#include "ISTabWidgetMain.h"
#include "ISInterfaceMetaForm.h"
#include "PMetaClassTable.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISWorkspaceForm : public ISParagraphBaseForm
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
	void CreateDuplicate(PMetaClassTable *MetaTable);

private:
	QVBoxLayout *Layout;
	ISTabWidgetMain *TabWidget;
	ISInterfaceMetaForm *CentralForm; //����������� ����� (�������)
	QString CurrentSubSystemUID;
};
//-----------------------------------------------------------------------------
