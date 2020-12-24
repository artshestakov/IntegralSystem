#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISParagraphBaseForm.h"
#include "ISTabWidgetMain.h"
#include "ISInterfaceMetaForm.h"
#include "PMetaClass.h"
//-----------------------------------------------------------------------------
class ISWorkspaceParagraph : public ISParagraphBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISWorkspaceParagraph(QWidget *parent = 0);
	virtual ~ISWorkspaceParagraph();

	void Invoke() override;

public slots:
	void AddObjectForm(QWidget *ObjectForm); //�������� ����� ������� �� ������ ���������

private:
	void ClickedSubSystem(const QString &SubSystemUID, const QIcon &IconSubSystem); //���������� ������� ����� �� ����������

private:
	QVBoxLayout *Layout;
	ISTabWidgetMain *TabWidget;
	ISInterfaceMetaForm *CentralForm; //����������� ����� (�������)
	QString CurrentSubSystemUID;
};
//-----------------------------------------------------------------------------
