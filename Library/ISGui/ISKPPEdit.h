#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
#include "ISLineEdit.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISKPPEdit : public ISLineEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISKPPEdit(QWidget *parent);
	virtual ~ISKPPEdit();
};
//-----------------------------------------------------------------------------
