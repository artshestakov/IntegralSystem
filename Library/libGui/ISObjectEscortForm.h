#pragma once
//-----------------------------------------------------------------------------
#include "libgui_global.h"
//-----------------------------------------------------------------------------
class LIBGUI_EXPORT ISObjectEscortForm : public QWidget
{
	Q_OBJECT

public:
	ISObjectEscortForm(int ParentObjectID, QWidget *parent = 0);
	virtual ~ISObjectEscortForm();
};
//-----------------------------------------------------------------------------
