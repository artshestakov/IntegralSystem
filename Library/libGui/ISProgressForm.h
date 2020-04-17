#pragma once
#ifndef _ISPROGRESSFORM_H_INCLUDED
#define _ISPROGRESSFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISProgressForm : public QProgressDialog
{
	Q_OBJECT

public:
	ISProgressForm(int Maximum, const QString &LabelText, QWidget *parent = 0, const QString &TitleText = QString());
	virtual ~ISProgressForm();

	void IncrementValue();
	bool WasCanceled() const;
	void SetCanceled(bool canceled);

protected:
	void showEvent(QShowEvent *e);

private:
	void CancelClicked();

private:
	QLabel *LabelStatus;
	int Value;
	bool Canceled;
};
//-----------------------------------------------------------------------------
#endif
