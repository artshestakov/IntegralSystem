#pragma once
//-----------------------------------------------------------------------------
#include "isfields_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISFIELDS_EXPORT ISLanguageEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISLanguageEdit(QWidget *parent);
	virtual ~ISLanguageEdit();
};
//-----------------------------------------------------------------------------
