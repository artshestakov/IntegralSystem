#pragma once
#ifndef _ISTOOLBUTTON_H_INCLUDED
#define _ISTOOLBUTTON_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISToolButton : public QToolButton
{
	Q_OBJECT

public:
	ISToolButton(QWidget *parent = 0);
	virtual ~ISToolButton();
};
//-----------------------------------------------------------------------------
#endif
