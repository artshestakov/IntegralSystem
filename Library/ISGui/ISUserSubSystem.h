#pragma once
//-----------------------------------------------------------------------------
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISUserSubSystem : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISUserSubSystem(QWidget *parent = 0);
	virtual ~ISUserSubSystem();
};
//-----------------------------------------------------------------------------
