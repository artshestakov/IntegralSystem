#pragma once
#ifndef _ISSETTINGFIELDWIDGETS_H_INCLUDED
#define _ISSETTINGFIELDWIDGETS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISFieldEdits.h"
//-----------------------------------------------------------------------------
class ISComboSettingEdit : public ISComboEdit
{
    Q_OBJECT

public:
    Q_INVOKABLE ISComboSettingEdit(QWidget *parent = 0);
    virtual ~ISComboSettingEdit();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboCalendarNoteType : public ISComboSettingEdit
{
    Q_OBJECT

public:
    Q_INVOKABLE ISComboCalendarNoteType(QWidget *parent = 0);
    virtual ~ISComboCalendarNoteType();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboTabBarSelectionBehavior : public ISComboSettingEdit
{
    Q_OBJECT

public:
    Q_INVOKABLE ISComboTabBarSelectionBehavior(QWidget *parent = 0);
    virtual ~ISComboTabBarSelectionBehavior();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboTableDoubleClickEvent : public ISComboSettingEdit
{
    Q_OBJECT

public:
    Q_INVOKABLE ISComboTableDoubleClickEvent(QWidget *parent = 0);
    virtual ~ISComboTableDoubleClickEvent();
};
//-----------------------------------------------------------------------------
#endif
