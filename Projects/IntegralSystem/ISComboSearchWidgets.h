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
    Q_INVOKABLE ISComboSearchBase(QWidget *parent);
    virtual ~ISComboSearchBase();

    ISNamespace::SearchType GetOperator();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboSearchNumber : public ISComboSearchBase
{
    Q_OBJECT

public:
    Q_INVOKABLE ISComboSearchNumber(QWidget *parent);
    virtual ~ISComboSearchNumber();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboSearchString : public ISComboSearchBase
{
    Q_OBJECT

public:
    Q_INVOKABLE ISComboSearchString(QWidget *parent);
    virtual ~ISComboSearchString();
};
//-----------------------------------------------------------------------------
#endif
