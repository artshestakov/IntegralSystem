#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISInterfaceForm.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISProcessForm : public ISInterfaceForm
{
	Q_OBJECT

public:
    ISProcessForm(const QString &Text = QString(), QWidget *parent = 0);
	virtual ~ISProcessForm();

	void SetText(const QString &Text);

protected:
	void showEvent(QShowEvent *e);

private:
	QLabel *LabelText;
};
//-----------------------------------------------------------------------------
