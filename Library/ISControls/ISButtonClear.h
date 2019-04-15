#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISButtonClear : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonClear(QWidget *parent = 0);
	virtual ~ISButtonClear();
};
//-----------------------------------------------------------------------------
