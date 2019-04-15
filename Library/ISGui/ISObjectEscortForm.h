#pragma once
//-----------------------------------------------------------------------------
#include "isgui_global.h"
//-----------------------------------------------------------------------------
class ISGUI_EXPORT ISObjectEscortForm : public QWidget
{
	Q_OBJECT

public:
	ISObjectEscortForm(int ParentObjectID, QWidget *parent = 0);
	virtual ~ISObjectEscortForm();
};
//-----------------------------------------------------------------------------
