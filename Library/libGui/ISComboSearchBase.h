#pragma once
#ifndef _ISCOMBOSEARCHBASE_H_INCLUDED
#define _ISCOMBOSEARCHBASE_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISComboEdit.h"
#include "ISNamespace.h"
//-----------------------------------------------------------------------------
class ISComboSearchBase : public ISComboEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSearchBase(QWidget *parent = 0);
	virtual ~ISComboSearchBase();

	ISNamespace::SearchOperatorType GetOperator();
};
//-----------------------------------------------------------------------------
#endif
