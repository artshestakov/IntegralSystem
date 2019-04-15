#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISComboStartedParagraph : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboStartedParagraph(QWidget *parent);
	virtual ~ISComboStartedParagraph();
};
//-----------------------------------------------------------------------------
