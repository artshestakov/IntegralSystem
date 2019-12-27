#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISLanguageEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISLanguageEdit(QWidget *parent);
	virtual ~ISLanguageEdit();
};
//-----------------------------------------------------------------------------
