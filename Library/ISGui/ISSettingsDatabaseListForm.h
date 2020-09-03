#pragma once
#ifndef _ISSETTINGSDATABASELISTFORM_H_INCLUDED
#define _ISSETTINGSDATABASELISTFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISListBaseForm.h"
//-----------------------------------------------------------------------------
class ISSettingsDatabaseListForm : public ISListBaseForm
{
	Q_OBJECT

public:
	Q_INVOKABLE ISSettingsDatabaseListForm(QWidget *parent = 0);
	virtual ~ISSettingsDatabaseListForm();

private:
	void Active();
};
//-----------------------------------------------------------------------------
#endif
