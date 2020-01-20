#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
//-----------------------------------------------------------------------------
class ISObjectEscortForm : public QWidget
{
	Q_OBJECT

public:
	ISObjectEscortForm(int ParentObjectID, QWidget *parent = 0);
	virtual ~ISObjectEscortForm();
};
//-----------------------------------------------------------------------------
