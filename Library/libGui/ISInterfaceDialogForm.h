#pragma once
#ifndef _ISINTERFACEDIALOGFORM_H_INCLUDED
#define _ISINTERFACEDIALOGFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "libGui_global.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT	ISInterfaceDialogForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void Accept();

public:
	ISInterfaceDialogForm(QWidget *parent = 0, Qt::WindowFlags Flags = 0);
	virtual ~ISInterfaceDialogForm();
	
	virtual bool Exec();
	virtual bool ExecAnimated();

protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void EscapeClicked() override;
	virtual void AfterShowEvent() override;

	void SetResult(bool result);

private:
	bool Result;
};
//-----------------------------------------------------------------------------
#endif
