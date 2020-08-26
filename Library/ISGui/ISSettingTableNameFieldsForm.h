#pragma once
#ifndef _ISSETTINGTABLENAMEFIELDSFORM_H_INCLUDED
#define _ISSETTINGTABLENAMEFIELDSFORM_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISInterfaceForm.h"
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISSettingTableNameFieldsForm : public ISInterfaceForm
{
	Q_OBJECT

public:
	ISSettingTableNameFieldsForm(QWidget *parent = 0);
	virtual ~ISSettingTableNameFieldsForm();

private:
	std::vector<ISLineEdit*> Edits;
};
//-----------------------------------------------------------------------------
#endif
