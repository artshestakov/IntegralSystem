#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISComboStartedParagraph : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboStartedParagraph(QWidget *parent);
	virtual ~ISComboStartedParagraph();
};
//-----------------------------------------------------------------------------
