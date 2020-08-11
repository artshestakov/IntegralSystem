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
	void AddObjectForm(QWidget *ObjectForm); //Добавить форму объекта на панель навигации

protected:
	void CreateSystems(); //Создание панели систем и подсистем
	void CreateTabWidget(); //Содание таба

	void ClickedSubSystem(const QString &SubSystemUID, const QIcon &IconSubSystem); //Обработчик события клика по подсистеме

private:
	QVBoxLayout *Layout;
	ISTabWidgetMain *TabWidget;
	ISInterfaceMetaForm *CentralForm; //Центральная форма (рабочая)
	QString CurrentSubSystemUID;
};
//-----------------------------------------------------------------------------
