#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISDivisionCodeEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISDivisionCodeEdit(QWidget *parent);
	virtual ~ISDivisionCodeEdit();

	bool IsValid() const;
};
//-----------------------------------------------------------------------------
