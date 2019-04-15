#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISParagraphBaseForm.h"
#include "ISListWidget.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISTelephonyForm : public ISParagraphBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISTelephonyForm(QWidget *parent = 0);
	virtual ~ISTelephonyForm();

	void Invoke() override;
	
protected:
	void CreateItem(const QIcon &Icon, const QString &Text, ISNamespace::TelephonyForm FormType);
	void ItemSelectionChanged();

private:
	QHBoxLayout *MainLayout;
	ISListWidget *ListWidget;
	QWidget *CentralWidget;
};
//-----------------------------------------------------------------------------
