#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISINNEdit : public ISLineEdit
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
