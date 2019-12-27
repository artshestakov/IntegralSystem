#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISINNEdit : public ISLineEdit
{
	Q_OBJECT

signals:
	void SearchFromINN();

public:
	Q_INVOKABLE ISINNEdit(QWidget *parent);
	virtual ~ISINNEdit();

	bool IsValid() const;
};
//-----------------------------------------------------------------------------
