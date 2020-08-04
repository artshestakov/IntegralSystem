#pragma once
#ifndef _ISSETTINGFIELDWIDGETS_H_INCLUDED
#define _ISSETTINGFIELDWIDGETS_H_INCLUDED
//-----------------------------------------------------------------------------
#include "ISComboEdit.h"
//-----------------------------------------------------------------------------
class ISCheckViewParagraph : public ISFieldEditBase
{
	Q_OBJECT

public:
	Q_INVOKABLE ISCheckViewParagraph(QWidget *parent = 0);
	virtual ~ISCheckViewParagraph();

	void SetValue(const QVariant &value) override;
	QVariant GetValue() const override;
	void Clear() override;

private:
	QButtonGroup *ButtonGroup;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
class ISComboIncomingFormPosition : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboIncomingFormPosition(QWidget *parent = 0);
	virtual ~ISComboIncomingFormPosition();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboStartedParagraph : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboStartedParagraph(QWidget *parent = 0);
	virtual ~ISComboStartedParagraph();
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
class ISComboTableDateFormat : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableDateFormat(QWidget *parent = 0);
	virtual ~ISComboTableDateFormat();
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboTableTimeFormat : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableTimeFormat(QWidget *parent = 0);
	virtual ~ISComboTableTimeFormat();
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
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ISComboTableSelectionBehavior : public ISComboSettingEdit
{
	Q_OBJECT

public:
	Q_INVOKABLE ISComboTableSelectionBehavior(QWidget *parent = 0);
	virtual ~ISComboTableSelectionBehavior();
};
//-----------------------------------------------------------------------------
#endif
