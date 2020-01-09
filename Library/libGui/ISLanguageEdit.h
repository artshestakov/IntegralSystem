#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISLanguageEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISLanguageEdit(QWidget *parent);
	virtual ~ISLanguageEdit();
};
//-----------------------------------------------------------------------------
