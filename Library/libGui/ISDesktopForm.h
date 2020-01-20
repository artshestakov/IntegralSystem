#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISParagraphBaseForm.h"
//-----------------------------------------------------------------------------
class ISDesktopForm : public ISParagraphBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDesktopForm(QWidget *parent = 0);
	virtual ~ISDesktopForm();

	void Invoke() override;

private:
	QVBoxLayout *MainLayout;
	QWidget *DesktopWidget;
};
//-----------------------------------------------------------------------------
