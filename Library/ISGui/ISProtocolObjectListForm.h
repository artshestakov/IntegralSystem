#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISProtocolSubSystem.h"
//-----------------------------------------------------------------------------
class ISProtocolObjectListForm : public ISProtocolSubSystem
{
	Q_OBJECT

public:
	Q_INVOKABLE ISProtocolObjectListForm(QWidget *parent = 0);
	virtual ~ISProtocolObjectListForm();

protected:
	virtual bool Update() override;
	void LoadData() override;
};
//-----------------------------------------------------------------------------
