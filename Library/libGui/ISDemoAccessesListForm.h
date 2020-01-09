#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISListBaseForm.h"
#include "ISDateEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISDemoAccessesListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDemoAccessesListForm(QWidget *parent = 0);
	virtual ~ISDemoAccessesListForm();

protected:
	void AgreementDateTime(const QVariant &value);

private:
	ISDateEdit *DateEdit;
};
//-----------------------------------------------------------------------------
