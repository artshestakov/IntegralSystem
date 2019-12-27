#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISLineEdit.h"
#include "ISServiceButton.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISUrlEdit : public ISLineEdit
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
