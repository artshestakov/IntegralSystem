#pragma once
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISProtocolListForm.h"
//-----------------------------------------------------------------------------
class ISProtocolObjectListForm : public ISProtocolListForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISProtocolObjectListForm(QWidget *parent = 0);
	virtual ~ISProtocolObjectListForm();

protected:
	virtual void LoadDataAfterEvent() override;
	void LoadData() override;
};
//-----------------------------------------------------------------------------
