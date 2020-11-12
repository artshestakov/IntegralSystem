#pragma once
#ifndef _ISASTERISKCALLSLISTFORM_H_INCLUDED
#define _ISASTERISKCALLSLISTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISAsteriskCallsListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISAsteriskCallsListForm(QWidget *parent = 0);
	virtual ~ISAsteriskCallsListForm();

private:
	void PlayRecordCall();
};
//-----------------------------------------------------------------------------
#endif
