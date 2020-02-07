#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISLineEdit.h"
#include "ISButtons.h"
//-----------------------------------------------------------------------------
class ISUrlEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUrlEdit(QWidget *parent = 0);
	virtual ~ISUrlEdit();

protected:
	void OpenUrl();
	void UrlChanged();

private:
	ISServiceButton *ButtonUrl;
};
//-----------------------------------------------------------------------------
