#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
#include "ISComboSettingEdit.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISComboStartedParagraph : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboStartedParagraph(QWidget *parent);
	virtual ~ISComboStartedParagraph();
};
//-----------------------------------------------------------------------------
