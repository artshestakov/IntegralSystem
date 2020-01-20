#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
#include "ISServiceButton.h"
//-----------------------------------------------------------------------------
class ISSearchEdit : public ISLineEdit
{
	Q_OBJECT

signals:
	void Search(const QString &SearchValue);

public:
	ISSearchEdit(QWidget *parent);
	virtual ~ISSearchEdit();

	void Updated();

protected:
	void SearchChanged();
	void AboutToShow();
	void AboutToHide();
	void LastSearchClicked();
	void Timeout();

private:
	ISServiceButton *ButtonLastSearch;
	QTimer *Timer;
};
//-----------------------------------------------------------------------------
