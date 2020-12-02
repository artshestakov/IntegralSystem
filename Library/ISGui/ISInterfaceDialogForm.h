#pragma once
#ifndef _ISINTERFACEDIALOGFORM_H_INCLUDED
#define _ISINTERFACEDIALOGFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT	ISInterfaceDialogForm : public ISInterfaceForm
{
	Q_OBJECT

signals:
	void Accept();

public:
	ISInterfaceDialogForm(bool HideCloseWindow = false, QWidget *parent = 0);
	virtual ~ISInterfaceDialogForm();
	
	virtual bool Exec();

protected:
	virtual void closeEvent(QCloseEvent *CloseEvent);
	virtual void EscapeClicked() override;
	virtual void AfterShowEvent() override;

	void SetResult(bool result);

private:
	bool Result;
	QEventLoop EventLoop;
};
//-----------------------------------------------------------------------------
#endif
