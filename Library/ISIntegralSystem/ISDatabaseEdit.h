#pragma once
//-----------------------------------------------------------------------------
#include "isintegralsystem_global.h"
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISINTEGRALSYSTEM_EXPORT ISDatabaseEdit : public ISComboEdit
{
	Q_OBJECT

public:
	ISDatabaseEdit(QWidget *parent = 0);
	virtual ~ISDatabaseEdit();
};
//-----------------------------------------------------------------------------
