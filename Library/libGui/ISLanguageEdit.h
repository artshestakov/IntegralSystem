#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISLanguageEdit : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISLanguageEdit(QWidget *parent);
	virtual ~ISLanguageEdit();
};
//-----------------------------------------------------------------------------
