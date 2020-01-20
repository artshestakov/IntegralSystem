#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISPushButton.h"
//-----------------------------------------------------------------------------
class ISButtonClose : public ISPushButton
{
	Q_OBJECT

public:
	ISButtonClose(QWidget *parent = 0);
	virtual ~ISButtonClose();
};
//-----------------------------------------------------------------------------
