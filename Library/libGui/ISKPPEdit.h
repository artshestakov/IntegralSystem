#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISKPPEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISKPPEdit(QWidget *parent);
	virtual ~ISKPPEdit();
};
//-----------------------------------------------------------------------------
