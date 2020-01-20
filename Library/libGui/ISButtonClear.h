#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class ISButtonClear : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonClear(QWidget *parent = 0);
	virtual ~ISButtonClear();
};
//-----------------------------------------------------------------------------
