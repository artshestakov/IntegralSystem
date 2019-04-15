#pragma once
//-----------------------------------------------------------------------------
#include "iscontrols_global.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class ISCONTROLS_EXPORT ISButtonClose : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonClose(QWidget *parent = 0);
	virtual ~ISButtonClose();
};
//-----------------------------------------------------------------------------
