#pragma once
#ifndef _ISCOMBOSEARCHWIDGETS_H_INCLUDED
#define _ISCOMBOSEARCHWIDGETS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ISFieldEdits.h"
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboSearchNumber : public ISComboSearchBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSearchNumber(QWidget *parent = 0);
	virtual ~ISComboSearchNumber();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboSearchString : public ISComboSearchBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboSearchString(QWidget *parent = 0);
	virtual ~ISComboSearchString();
};
//-----------------------------------------------------------------------------
#endif
